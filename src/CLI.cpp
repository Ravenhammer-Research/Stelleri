/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "CLI.hpp"
#include "CommandDispatcher.hpp"
#include "DeleteCommand.hpp"
#include "Parser.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

CLI::CLI(std::unique_ptr<ConfigurationManager> mgr)
    : mgr_(std::move(mgr)), el_(nullptr), hist_(nullptr) {
  // Set history file to ~/.netcli_history
  const char *home = getenv("HOME");
  if (home) {
    historyFile_ = std::string(home) + "/.netcli_history";
  }
}

CLI::~CLI() { cleanupEditLine(); }

void CLI::loadHistory() {
  if (!hist_ || historyFile_.empty())
    return;
  // Load history from file
  history(hist_, &ev_, H_LOAD, historyFile_.c_str());
}

void CLI::saveHistory(const std::string &line) {
  if (!hist_ || historyFile_.empty() || line.empty())
    return;
  // Add to history and save to file
  history(hist_, &ev_, H_ENTER, line.c_str());
  history(hist_, &ev_, H_SAVE, historyFile_.c_str());
}

void CLI::processLine(const std::string &line) {
  if (line.empty())
    return;
  if (line == "exit" || line == "quit") {
    std::exit(0);
  }

  // Save to history but do not perform any system changes here — stubbed.
  saveHistory(line);

  netcli::Parser parser;
  auto toks = parser.tokenize(line);
  auto cmd = parser.parse(toks);
  if (!cmd || !cmd->head()) {
    std::cerr << "Error: Invalid command\n";
    return;
  }

  // Dispatch parsed command via CommandDispatcher
  netcli::CommandDispatcher dispatcher;
  dispatcher.dispatch(cmd->head(), mgr_.get());
}

char *CLI::promptFunc(EditLine *el) {
  (void)el; // unused
  static char prompt[] = "net> ";
  return prompt;
}

void CLI::setupEditLine() {
  // Initialize editline
  el_ = el_init("net", stdin, stdout, stderr);
  if (!el_) {
    std::cerr << "Failed to initialize editline\n";
    return;
  }

  // Set prompt
  el_set(el_, EL_PROMPT, &CLI::promptFunc);

  // Enable editor (emacs mode by default)
  el_set(el_, EL_EDITOR, "emacs");

  // Set up completion callback
  el_set(el_, EL_ADDFN, "ed-complete", "Complete command", &CLI::completeCommand);
  el_set(el_, EL_BIND, "^I", "ed-complete", nullptr); // Tab key

  // Store this pointer for completion callback
  el_set(el_, EL_CLIENTDATA, this);

  // Initialize history
  hist_ = history_init();
  if (hist_) {
    history(hist_, &ev_, H_SETSIZE, 1000);
    el_set(el_, EL_HIST, history, hist_);
    loadHistory();
  }
}

void CLI::cleanupEditLine() {
  if (hist_) {
    history_end(hist_);
    hist_ = nullptr;
  }
  if (el_) {
    el_end(el_);
    el_ = nullptr;
  }
}

std::vector<std::string>
CLI::getCompletions(const std::vector<std::string> &tokens,
                    const std::string &partial) const {
  if (tokens.empty()) {
    // At start of line: offer top-level commands
    std::vector<std::string> topLevel = {"show", "set", "delete", "exit", "quit"};
    std::vector<std::string> matches;
    for (const auto &cmd : topLevel) {
      if (cmd.rfind(partial, 0) == 0)
        matches.push_back(cmd);
    }
    return matches;
  }

  // Try to parse the tokens so far to build a token chain
  auto cmd = parser_.parse(tokens);
  if (!cmd || !cmd->head())
    return {};

  // Traverse to the tail of the token chain and call autoComplete
  auto tok = cmd->head();
  while (tok && tok->hasNext())
    tok = tok->getNext();

  return tok ? tok->autoComplete(partial) : std::vector<std::string>{};
}

unsigned char CLI::completeCommand(EditLine *el, int ch) {
  (void)ch; // unused

  // Get CLI instance from client data
  CLI *cli = nullptr;
  el_get(el, EL_CLIENTDATA, &cli);
  if (!cli)
    return CC_ERROR;

  // Get current line info
  const LineInfo *li = el_line(el);
  if (!li)
    return CC_ERROR;

  // Extract word before cursor
  const char *cursor = li->cursor;
  const char *start = li->buffer;
  const char *word_start = cursor;

  // Find word boundary (space or start of line)
  while (word_start > start && word_start[-1] != ' ')
    word_start--;

  std::string partial(word_start, cursor - word_start);

  // Tokenize everything before the current word
  std::string lineBeforeWord(start, word_start);
  auto tokens = cli->parser_.tokenize(lineBeforeWord);

  auto completions = cli->getCompletions(tokens, partial);

  if (completions.empty()) {
    el_beep(el);
    return CC_REFRESH;
  }

  if (completions.size() == 1) {
    // Single match: complete it
    const std::string &match = completions[0];
    std::string insertion = match.substr(partial.size());
    if (!insertion.empty()) {
      el_insertstr(el, insertion.c_str());
    }
    return CC_REFRESH;
  }

  // Multiple matches: display above prompt
  std::cout << '\n';
  for (const auto &comp : completions) {
    std::cout << comp << "  ";
  }
  std::cout << "\n";

  return CC_REDISPLAY;
}

void CLI::run() {
  // Check if in interactive mode (tty)
  if (!isatty(STDIN_FILENO)) {
    // Non-interactive: read from stdin line by line
    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty() || line[0] == '#')
        continue;
      processLine(line);
    }
    return;
  }

  // Interactive mode with editline
  setupEditLine();
  if (!el_) {
    std::cerr << "Failed to initialize interactive mode\n";
    return;
  }

  int count;
  const char *line;
  while ((line = el_gets(el_, &count)) != nullptr && count > 0) {
    std::string cmd(line);
    // Remove trailing newline
    if (!cmd.empty() && cmd.back() == '\n')
      cmd.pop_back();

    if (cmd.empty())
      continue;

    if (cmd == "exit" || cmd == "quit")
      break;

    saveHistory(cmd);
    processLine(cmd);
  }
}
