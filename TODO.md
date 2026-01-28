# NetCLI - Network Configuration Tool for FreeBSD

## Architecture and File Structure

A C++26 command-line network configuration tool supporting VyOS-style commands with an extensible token-based architecture.

---

## Core Token System

### 1. Base Classes

-#### `include/Token.hpp` / `src/Token.cpp`
- Abstract base class for all tokens
- Virtual `toString()` method that returns the CLI fragment for this token (used to reconstruct commands)
- Virtual `autoComplete(std::string_view partial) -> std::vector<std::string>` for CLI tab completion
- `std::shared_ptr<Token> next` for linked list navigation
- `virtual std::unique_ptr<Token> clone() const = 0` for token duplication
- Methods: `setNext()`, `getNext()`, `hasNext()`
- **Implementation**: Define pure virtual methods, use `std::enable_shared_from_this<Token>`
- **Note**: Each subclass implements `autoComplete()` to suggest valid next tokens/values

#### `include/Command.hpp` / `src/Command.cpp`
- Container for token linked list
- `std::shared_ptr<Token> head` (first token)
- Methods: `addToken()`, `toString()`, `execute()`, `validate()`
- Owns the entire token chain
- **Implementation**: 
  - `addToken()`: Append to end of linked list, maintain tail pointer for O(1) append
  - `toString()`: Traverse list calling `toString()` on each token for debugging/display
  - `execute()`: Delegate to ConfigurationManager based on command type
  - `validate()`: Walk chain ensuring proper token sequence
- **Use**: Smart pointer for automatic memory management

---

## Command Type Tokens

### 3. `include/SetCommand.hpp` / `src/SetCommand.cpp`
- Token for "set" operations (create/update)
- Stores command type as enum or string
- **Implementation**:
  - Inherits from Token
  - `toString()` returns "set" for display
  - `autoComplete()` returns valid keywords after "set" (interfaces, vrf, protocols)
- **Pattern**: Command pattern for undo/redo support later

### 4. `include/ShowCommand.hpp` / `src/ShowCommand.cpp`
- Token for "show" operations (display config)
- May contain format hints (table, summary, detail)
- **Implementation**:
  - `toString()` returns "show" for display
  - `autoComplete()` returns: ["interfaces", "vrf", "protocols", "route"]
  - Detects format from tokens ("brief" → summary, default → table)
- **Integration**: Works with TableFormatter/SummaryFormatter based on context

### 5. `include/DeleteCommand.hpp` / `src/DeleteCommand.cpp`
- Token for "delete" operations (remove config)
- **Implementation**:
  - `toString()` returns "delete" for display
  - `autoComplete()` uses ConfigurationManager to suggest only existing paths
  - Validation ensures path exists before deletion
- **Safety**: Should prompt for confirmation on critical deletes (entire interfaces, VRFs)

---

## Network Configuration Tokens

### 6. `include/InterfaceToken.hpp` / `src/InterfaceToken.cpp`
- Handles: `interfaces ethernet eth1`, `interfaces loopback lo`, `interfaces pppoe pppoe0`
- Stores: interface type (ethernet/loopback/pppoe/vlan), interface name
- FreeBSD support: epair, vlan, lagg, bridge, wg, tap, wlan, etc.
- Contains optional VRF field (`std::optional<std::string> vrf`) and source-interface field
- **Implementation**:
  - Store type as `enum class InterfaceType` and name as `std::string`
  - Parse VLAN notation: "eth2.150" → name="eth2.150", parent="eth2", vlan=150
  - `toString()` returns display representation: "interfaces ethernet eth1"
  - `autoComplete()` suggests interface types, then existing interface names from config
  - Methods: `getType()`, `getName()`, `getVRF()`, `getSourceInterface()`, `setVRF()`, `setSourceInterface()`

### 7. `include/AddressToken.hpp` / `src/AddressToken.cpp`
- Handles: `address '10.100.100.1/24'`
- Stores: IP address with CIDR notation
- Supports IPv4 and IPv6
- **Implementation**:
  - Store as `std::string` (raw CIDR) or parse into IP + prefix using `struct in_addr`/`in6_addr`
  - `toString()` returns: "address '10.100.100.1/24'"
  - `autoComplete()` returns empty (user types IP manually)
  - Validation: Use `ValidationUtils::isValidCIDR()` to check format
- **Consider**: Using `<arpa/inet.h>` for `inet_pton()` validation
- **Storage**: Keep both string representation and parsed components

### 8. `include/VRFToken.hpp` / `src/VRFToken.cpp`
- Handles: `vrf 'MGMT'` or `vrf name LAN`
- Stores: VRF name (maps to FreeBSD FIB)
- Attributes: table number (`std::optional<int>`), bind-to-all flag (`bool`)
- **Implementation**:
  - Parse both forms: "vrf 'MGMT'" and "vrf name LAN"
  - Store name, table number, and flags
  - `toString()` returns display representation based on context
  - `autoComplete()` suggests existing VRF names from config + "name" keyword
  - Table number mapping: Store in ConfigurationManager, default FIB ranges 100-255
- **FreeBSD**: FIB numbers are kernel routing tables (for future ioctl integration)

### 9. `include/ProtocolsToken.hpp` / `src/ProtocolsToken.cpp`
- Handles: `protocols static`
- Stores: protocol type (static routing initially)
- **Implementation**:
  - Store protocol as `enum class ProtocolType { Static, BGP, OSPF, ... }`
  - `toString()` returns "protocols static"
  - `autoComplete()` returns ["static"] (later: bgp, ospf)
  - Validates that protocol type is supported
- **Extensibility**: Easy to add BGP, OSPF by extending enum and switch cases

### 10. `include/RouteToken.hpp` / `src/RouteToken.cpp`
- Handles: `route 0.0.0.0/0`, `route 10.150.150.0/24`
- Stores: destination prefix, next-hop/interface, blackhole flag
- Methods for route attributes (interface, next-hop, vrf)
- **Implementation**:
  - Store: `std::string prefix`, `std::optional<std::string> nexthop`, `std::optional<std::string> interface`, `std::optional<std::string> vrf`, `bool blackhole`
  - Parse prefix using CIDR validation
  - `toString()` returns "route 0.0.0.0/0"
  - `autoComplete()` suggests "interface", "next-hop", "blackhole", "vrf" as next keywords
  - Complex token: Needs to parse variable-length attributes
- **Parsing Logic**: State machine to handle "interface X vrf Y" sequences

### 11. `include/NextHopToken.hpp` / `src/NextHopToken.cpp`
- Handles: `next-hop 10.1.1.1`
- Stores: next-hop IP address
- **Implementation**:
  - Store IP as `std::string` (validated)
  - `toString()` returns "next-hop 10.1.1.1"
  - `autoComplete()` returns empty (user provides IP)
  - Validate IP format using `ValidationUtils::isValidIP()`
- **Use**: Attached to RouteToken as attribute

### 15. `include/DistanceToken.hpp` / `src/DistanceToken.cpp`
- Dont add this for now 
- Handles: `distance '1'`
- Stores: administrative distance for routes

### 16. `include/BlackholeToken.hpp` / `src/BlackholeToken.cpp`
- Handles: `blackhole`
- Boolean flag token- **Implementation**:
  - Simple token with no additional data
  - `toString()` returns "blackhole" for display
  - `autoComplete()` returns empty (complete keyword)
  - Used by RouteToken to mark null routes

---

## Parsing and Tokenization

### 18. `include/Tokenizer.hpp` / `src/Tokenizer.cpp`
- Lexical analyzer
- Methods: `tokenize(std::string_view input)` → `std::vector<std::string>` (or vector of string_views)
- Handles quoted strings, whitespace, special characters
- Uses `std::string_view` for efficiency
- **Implementation**:
  - State machine: NORMAL, IN_QUOTE, ESCAPE
  - Split on whitespace unless inside quotes
  - Handle escapes: \\ → \, \' → '
  - Preserve quoted strings: 'hello world' → single token "hello world"
  - Strip outer quotes from result tokens
- **Example**: "set vrf name 'MY VRF'" → ["set", "vrf", "name", "MY VRF"]
- **Edge Cases**: Empty strings, nested quotes, trailing whitespace

### 14. `include/Parser.hpp` / `src/Parser.cpp`
- Converts tokenized strings into Token objects
- Methods: `parse(std::span<const std::string> tokens)` → `std::unique_ptr<Command>`
- Builds linked list of tokens
- Validates command structure
- Factory pattern for creating appropriate token types
- **Implementation**:
  - Stateful parser with token position tracking
  - First token determines command type (set/show/delete)
  - Use CommandFactory to create command token
  - Recursive descent or table-driven parsing for remaining tokens
  - Build Token chain: command → context tokens → value tokens
  - Error handling: throw `ParserException` with position info
- **Algorithm**:
  1. Check first token → create SetCommand/ShowCommand/DeleteCommand
  2. Iterate remaining tokens, match against grammar
  3. Create appropriate Token subclass for each keyword/value
  4. Link tokens via `setNext()`
- **Grammar Awareness**: Parser knows valid sequences (interfaces → type → name → address)

### 15. `include/CommandFactory.hpp` / `src/CommandFactory.cpp`
- Factory for creating command tokens (set/show/delete)
- Registry pattern for extensibility
- **Implementation**:
  - Static factory method: `createCommand(std::string_view type)` → `std::unique_ptr<Token>`
  - Map of string → factory functions
  - Registry: `std::unordered_map<std::string_view, std::function<std::unique_ptr<Token>()>>`
  - Register all command types at startup
- **Usage**: `auto cmd = CommandFactory::create("set");`
- **Extensibility**: Easy to add new commands without modifying Parser

---

## Output Formatting

### 21. `include/TableFormatter.hpp` / `src/TableFormatter.cpp`
- Base class for table formatters
- Formats output as ASCII tables
- Methods: `format(const ConfigData& data)` → std::string
- Column alignment, borders, headers
- Used for: `show interfaces`, `show vrf`
- **Implementation**:
  - Abstract base with `virtual format()` method
  - Helper methods: `calculateColumnWidths()`, `renderHeader()`, `renderRow()`, `renderBorder()`
  - Use box-drawing characters or ASCII: +---+---+ or ┌───┬───┐
  - Column alignment: left, right, center via enum
  - Dynamic width calculation based on content
- **Data Structure**: Accepts vector of rows, each row is vector of strings

### 16a. `include/InterfaceTableFormatter.hpp` / `src/InterfaceTableFormatter.cpp`
- Subclass of TableFormatter
- Specialized for interface display
- **Implementation**:
  - Columns: Name, Type, Address, VRF, Status, MTU
  - Overrides `format()` to structure interface data
  - Example output:
    ```
    Interface    Type       Address              VRF      Status  MTU
    -----------  ---------  -------------------  -------  ------  ----
    eth1         ethernet   10.100.100.1/24      MGMT     UP      1500
    eth2.150     vlan       10.150.150.1/24      LAN      UP      1500
    ```
- **Integration**: ConfigurationManager returns interface list, formatter renders it

### 16b. `include/RouteTableFormatter.hpp` / `src/RouteTableFormatter.cpp`
- Subclass of TableFormatter  
- Specialized for routing table display
- **Implementation**:
  - Columns: Destination, Gateway, Interface, VRF, Flags
  - Group routes by VRF optionally
  - Example output:
    ```
    Destination       Gateway        Interface    VRF      Flags
    ----------------  -------------  -----------  -------  -----
    0.0.0.0/0         10.1.1.1       pppoe0       WAN      UG
    10.0.0.0/8        -              -            -        B
    ```
  - Flags: U=Up, G=Gateway, B=Blackhole
- **Sorting**: Sort routes by prefix length, then lexicographically

### 22. `include/SummaryFormatter.hpp` / `src/SummaryFormatter.cpp`
- Formats output as summaries/lists
- Methods: `format(const ConfigData& data)` → std::string
- Condensed view
- Used for: `show interfaces brief`- **Implementation**:
  - Minimal columns, one-line per item
  - Truncate long fields with ellipsis
  - Example: `eth1 (UP) 10.100.100.1/24 [MGMT]`
  - Focus on status and key identifiers
  - Optional color coding: green=UP, red=DOWN
- **Use Cases**: Quick overview, scripting output
### 23. `include/Formatter.hpp` / `src/Formatter.cpp`
- Abstract base class for formatters
- Virtual `format()` method
- Handles indentation, colors (optional)- **Implementation**:
  - Pure virtual: `virtual std::string format(const ConfigData& data) const = 0`
  - Protected helpers: `indent(int level)`, `colorize(std::string_view text, Color c)`
  - Color support via ANSI escape codes (detect TTY with `isatty()`)
  - Factory method: `Formatter::create(FormatType type)` → unique_ptr
- **Pattern**: Strategy pattern for interchangeable formatting
---

## Configuration Management

### 19. `include/ConfigurationManager.hpp` / `src/ConfigurationManager.cpp`
- Abstract base class for configuration management
- Allows swapping implementations (memory, file, remote, etc.)
- NETCONF-inspired interface design
- **Abstract Methods** (to be implemented by subclasses):
  - `virtual void set(const std::string& path, const ConfigData& data) = 0`: Creates/updates config nodes
  - `virtual void delete_config(const std::string& path) = 0`: Removes node and children
- **Concrete Methods** (implemented in base class):
  - `std::optional<ConfigData> show(const std::string& path) const`: Query configuration data
  - `std::vector<ConfigData> show_all(const std::string& path_prefix) const`: Query multiple items
- **NETCONF Operations** (for future implementation):
  - Session Management: `hello()`, `kill_session()`, `close_session()`
  - Configuration: `edit_config()`, `copy_config()`, `delete_config()`, `lock()`, `unlock()`, `commit()`, `discard_changes()`
  - Data Retrieval: `get()`, `get_config()`
- **Implementation**:
  - Pure abstract base class with virtual destructor
  - Factory method: `static std::unique_ptr<ConfigurationManager> create(Type type)`
  - No singleton pattern - allows multiple instances for different backends
  - Path-based access: "interfaces.ethernet.eth1.address" → value
- **Note**: Only `set()`, `delete_config()`, and `show()` are used in initial implementation

### 19a. `include/MemoryConfigurationManager.hpp` / `src/MemoryConfigurationManager.cpp`
- Concrete implementation using in-memory storage
- Inherits from ConfigurationManager
- **Implementation**:
  - Config tree: `std::map<std::string, ConfigNode>` (ordered for display)
  - `set()`: Creates intermediate nodes if missing, stores ConfigData
  - `delete_config()`: Removes node and children from tree
  - `show()`: Returns ConfigData by path lookup
  - Optional features: `commit()`, `rollback()` with history stack
- **Structure**: Tree nodes contain ConfigData (variant of interface/vrf/route objects)
- **Validation**: Check dependencies (e.g., VRF must exist before assigning to interface)
- **Future**: Add `commit()` to apply changes using FreeBSD network ioctls from /usr/include (SIOCAIFADDR, SIOCSIFADDR, etc.)

### 20. `include/ConfigData.hpp` / `src/ConfigData.cpp`
- Data structure for configuration items
- Represents interfaces, VRFs, routes, etc.
- Uses `std::variant`, `std::optional` for flexible data storage
- **Implementation**:
  - Define structs: `InterfaceConfig`, `VRFConfig`, `RouteConfig`, `StaticRouteConfig`
  - Use variant: `using ConfigValue = std::variant<InterfaceConfig, VRFConfig, RouteConfig, ...>`
  - Optional fields: `std::optional<std::string> address`, `std::optional<int> mtu`
  - Methods: `std::string toString()`, `bool isValid()`
- **Example**:
  ```cpp
  struct InterfaceConfig {
      std::string name;
      std::string type;
      std::optional<std::string> address;
      std::optional<std::string> vrf;
      int mtu = 1500;
  };
  ```
- **Serialization**: Later add JSON/XML export for persistence

---

## CLI Interface

### 21. `include/CLI.hpp` / `src/CLI.cpp`
- Interactive CLI using GNU readline
- Methods: `run()`, `processCommand()`, `setupReadline()`
- Command history, tab completion
- Prompt customization
- Signal handling (Ctrl+C, Ctrl+D)
- **Implementation**:
  - `run()`: Main REPL loop using `readline(prompt)`
  - `processCommand()`: Tokenize → Parse → Execute
  - History: `add_history()` for successful commands, save to `~/.netcli_history`
  - Tab completion: Register callback with `rl_attempted_completion_function`
  - Prompt: Configurable, show config mode or user@host
  - Signal handling: `signal(SIGINT, handler)` to prevent abrupt exit
  - Error handling: Catch exceptions, display user-friendly messages
- **Readline Integration**:
  ```cpp
  void setupReadline() {
      rl_attempted_completion_function = command_completion;
      using_history();
      read_history(history_file.c_str());
  }
  ```
- **Exit**: Handle EOF (Ctrl+D), "exit" command, "quit" command

### 22. `include/CLICompletion.hpp` / `src/CLICompletion.cpp`
- Tab completion for commands
- Context-aware suggestions
- Integrates with readline
- **Implementation**:
  - Callback: `char** command_completion(const char* text, int start, int end)`
  - Tokenize partial input up to cursor position
  - Parse partial command to determine context
  - Call `Token::autoComplete()` on last parsed token
  - Return matches as null-terminated array of char*
- **Context Awareness**:
  - "set " → suggest ["interfaces", "vrf", "protocols"]
  - "set interfaces " → suggest ["ethernet", "loopback", "pppoe"]
  - "set interfaces ethernet " → suggest existing interfaces + common names
  - "set interfaces ethernet eth1 " → suggest ["address", "vrf", "mtu"]
- **Algorithm**:
  1. Get current line buffer and cursor position
  2. Tokenize up to cursor
  3. Partial parse to get context
  4. Query Parser or Token for valid completions
  5. Filter by partial token (text parameter)
  6. Return as readline-compatible array
- **Helper**: `std::vector<std::string> getCompletions(std::span<std::string> tokens)`

---

## Main Application

### 23. `src/main.cpp`
- Entry point
- Uses `getopt_long()` for command-line arguments
- Options: `-c` (one-shot command), `-f` (file input), `-i` (interactive)
- Initializes CLI or processes one-shot commands
- Error handling and exit codes
- **Implementation**:
  ```cpp
  int main(int argc, char* argv[]) {
      struct option long_options[] = {
          {"command", required_argument, 0, 'c'},
          {"file", required_argument, 0, 'f'},
          {"interactive", no_argument, 0, 'i'},
          {"help", no_argument, 0, 'h'},
          {0, 0, 0, 0}
      };
      // Parse args, create CLI, run mode
  }
  ```
- **Modes**:
  - No args: Interactive mode (default)
  - `-c "command"`: Execute single command and exit
  - `-f file.conf`: Read and execute commands from file
  - `-i`: Force interactive even with stdin redirect
- **Error Codes**: 0=success, 1=syntax error, 2=execution error, 3=file error
- **Help**: `-h` or `--help` displays usage

---

## Build System

### 30. `CMakeLists.txt`
- C++26 standard
- FindReadline module
- Compiler flags: `-Wall -Wextra -pedantic`
- Build types: Debug, Release
- Install targets- **Implementation**:
  ```cmake
  cmake_minimum_required(VERSION 3.25)
  project(netcli VERSION 1.0.0 LANGUAGES CXX)
  
  set(CMAKE_CXX_STANDARD 26)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  
  # Find readline
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
  find_package(Readline REQUIRED)
  
  # Sources
  file(GLOB_RECURSE SOURCES "src/*.cpp")
  
  # Executable
  add_executable(netcli ${SOURCES})
  target_include_directories(netcli PRIVATE include)
  target_link_libraries(netcli PRIVATE Readline::Readline)
  
  # Compiler flags
  target_compile_options(netcli PRIVATE -Wall -Wextra -pedantic -Werror)
  
  # Install
  install(TARGETS netcli DESTINATION bin)
  ```
- **Debug Build**: Add `-g -O0 -DDEBUG` flags
- **Release Build**: Add `-O3 -DNDEBUG` flags
### 31. `cmake/FindReadline.cmake`
- CMake module to find GNU readline library on FreeBSD- **Implementation**:
  ```cmake
  # FindReadline.cmake
  find_path(Readline_INCLUDE_DIR readline/readline.h
      PATHS /usr/include /usr/local/include)
  
  find_library(Readline_LIBRARY readline
      PATHS /usr/lib /usr/local/lib)
  
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Readline
      REQUIRED_VARS Readline_LIBRARY Readline_INCLUDE_DIR)
  
  if(Readline_FOUND AND NOT TARGET Readline::Readline)
      add_library(Readline::Readline UNKNOWN IMPORTED)
      set_target_properties(Readline::Readline PROPERTIES
          IMPORTED_LOCATION "${Readline_LIBRARY}"
          INTERFACE_INCLUDE_DIRECTORIES "${Readline_INCLUDE_DIR}")
  endif()
  ```
- **FreeBSD**: Readline typically in `/usr/local/lib` and `/usr/local/include`
---

## Utilities

### 32. `include/StringUtils.hpp` / `src/StringUtils.cpp`
- Helper functions: `trim()`, `split()`, `join()`, `unquote()`
- Uses `std::string_view` for efficiency- **Implementation**:
  ```cpp
  namespace StringUtils {
      std::string_view trim(std::string_view str);
      std::vector<std::string_view> split(std::string_view str, char delim);
      std::string join(std::span<const std::string> parts, std::string_view delim);
      std::string unquote(std::string_view str);  // Remove surrounding quotes
      bool startsWith(std::string_view str, std::string_view prefix);
      bool endsWith(std::string_view str, std::string_view suffix);
      std::string toLower(std::string_view str);
      std::string toUpper(std::string_view str);
  }
  ```
- **Efficiency**: Return views when possible, copy only when necessary
- **Edge Cases**: Handle empty strings, multiple delimiters, Unicode (future)
### 33. `include/ValidationUtils.hpp` / `src/ValidationUtils.cpp`
- Validation functions: `isValidIP()`, `isValidCIDR()`, `isValidInterface()`
- IPv4/IPv6 validation- **Implementation**:
  ```cpp
  namespace ValidationUtils {
      bool isValidIPv4(std::string_view ip);
      bool isValidIPv6(std::string_view ip);
      bool isValidIP(std::string_view ip);  // Either v4 or v6
      bool isValidCIDR(std::string_view cidr);  // IP/prefix
      bool isValidInterface(std::string_view name);  // Match pattern
      bool isValidVRFName(std::string_view name);
      std::optional<int> extractPrefixLength(std::string_view cidr);
  }
  ```
- **IPv4**: Use `inet_pton(AF_INET, ...)` from `<arpa/inet.h>`
- **IPv6**: Use `inet_pton(AF_INET6, ...)`
- **CIDR**: Split on '/', validate IP and numeric prefix
- **Interface**: Regex or manual check for valid characters (alphanumeric, dot, dash)
- **Return**: Bool for validity checks, optional for parsed values
---

## Directory Structure

```
netcli/
├── CMakeLists.txt
├── cmake/
│   └── FindReadline.cmake
├── include/
│   ├── Token.hpp
│   ├── Command.hpp
│   ├── SetCommand.hpp
│   ├── ShowCommand.hpp
│   ├── DeleteCommand.hpp
│   ├── InterfaceToken.hpp
│   ├── AddressToken.hpp
│   ├── VRFToken.hpp
│   ├── ProtocolsToken.hpp
│   ├── RouteToken.hpp
│   ├── NextHopToken.hpp
│   ├── BlackholeToken.hpp
│   ├── Tokenizer.hpp
│   ├── Parser.hpp
│   ├── CommandFactory.hpp
│   ├── Formatter.hpp
│   ├── TableFormatter.hpp
│   ├── InterfaceTableFormatter.hpp
│   ├── RouteTableFormatter.hpp
│   ├── SummaryFormatter.hpp
│   ├── ConfigurationManager.hpp
│   ├── MemoryConfigurationManager.hpp
│   ├── ConfigData.hpp
│   ├── CLI.hpp
│   ├── CLICompletion.hpp
│   ├── StringUtils.hpp
│   └── ValidationUtils.hpp
├── src/
│   ├── Token.cpp
│   ├── Command.cpp
│   ├── SetCommand.cpp
│   ├── ShowCommand.cpp
│   ├── DeleteCommand.cpp
│   ├── InterfaceToken.cpp
│   ├── AddressToken.cpp
│   ├── VRFToken.cpp
│   ├── ProtocolsToken.cpp
│   ├── RouteToken.cpp
│   ├── NextHopToken.cpp
│   ├── BlackholeToken.cpp
│   ├── Tokenizer.cpp
│   ├── Parser.cpp
│   ├── CommandFactory.cpp
│   ├── Formatter.cpp
│   ├── TableFormatter.cpp
│   ├── InterfaceTableFormatter.cpp
│   ├── RouteTableFormatter.cpp
│   ├── SummaryFormatter.cpp
│   ├── ConfigurationManager.cpp
│   ├── MemoryConfigurationManager.cpp
│   ├── ConfigData.cpp
│   ├── CLI.cpp
│   ├── CLICompletion.cpp
│   ├── StringUtils.cpp
│   ├── ValidationUtils.cpp
│   └── main.cpp
└── README.md
```

---

## Example Token Chain

For command: `set vrf name LAN protocols static route 172.16.20.0/24 1 eth2.3500 vrf 'PROD'`

```
Command
  ↓
SetCommand("set")
  ↓
VRFToken("vrf", "LAN")
  ↓
ProtocolsToken("protocols", "static")
  ↓
RouteToken("route", "172.16.20.0/24")
  ↓
InterfaceToken("interface", "eth2.3500")
  ↓
VRFToken("vrf", "PROD")
```

Calling `command->toString()` traverses the chain and reconstructs the original command.

---

## Key Design Decisions

1. **Smart Pointers**: All tokens use `std::shared_ptr` for the chain, `std::unique_ptr` for ownership
2. **String Views**: Parser uses `std::string_view` to avoid copies
3. **Spans**: Token collections use `std::span` for non-owning references
4. **Optionals**: Token attributes use `std::optional<T>` for optional values
5. **FreeBSD FIB**: VRF names map to FreeBSD FIB (Forwarding Information Base) numbers
6. **Extensibility**: Factory pattern allows easy addition of new token types
7. **Immutability**: Tokens are immutable once created; changes create new command chains

---

## Example Commands Supported

```bash
# Interface configuration
set interfaces ethernet eth1 address '10.100.100.1/24'
set interfaces ethernet eth1 vrf 'MGMT'
set interfaces ethernet eth2.150 address '10.150.150.1/24'
set interfaces ethernet eth2.150 vrf 'LAN'
set interfaces loopback lo
set interfaces pppoe pppoe0 vrf 'WAN'

# VRF configuration
set vrf bind-to-all
set vrf name LAN table '103'
set vrf name MGMT table '102'
set vrf name PROD table '104'
set vrf name WAN table '101'

# Static routing (VRF-aware)
set vrf name LAN protocols static route 0.0.0.0/0 interface pppoe0 vrf 'WAN'
set vrf name LAN protocols static route 10.100.100.0/24 interface eth1 vrf 'MGMT'
set vrf name LAN protocols static route 172.16.20.0/24 interface eth2.3500 vrf 'PROD'

# Static routing (global)
set protocols static route 0.0.0.0/0 next-hop 10.1.1.1
set protocols static route 10.0.0.0/8 blackhole
set protocols static route 172.16.0.0/12 blackhole
set protocols static route 192.168.0.0/16 blackhole
```

---

## FreeBSD Interface Support

The tool supports FreeBSD-specific interface types:
- `ethernet` - Standard Ethernet interfaces (e.g., em0, igb0, ix0)
- `epair` - Virtual Ethernet pair interfaces (epair0a, epair0b)
- `vlan` - VLAN interfaces (vlan48, vlan.1)
- `lagg` - Link aggregation interfaces (lagg0, lagg0.16)
- `bridge` - Bridge interfaces (bridge0)
- `wg` - WireGuard interfaces (wg0)
- `tap` - TAP interfaces (tap0)
- `wlan` - Wireless interfaces (wlan0)
- `loopback` - Loopback interfaces (lo0, lo255)
- `pppoe` - PPPoE interfaces

**VRF Mapping**: VRF names map to FreeBSD FIB (Forwarding Information Base) numbers
- `fib: 48` = VRF name might be "CUSTOMER_A"
- `tunnelfib: 255` = Tunnel VRF for WireGuard, etc.

---

## Implementation Status

- [ ] Core token system (Token, Command)
- [ ] Command type tokens (Set, Show, Delete)
- [ ] Network configuration tokens
- [ ] Tokenizer and Parser
- [ ] Output formatters
- [ ] Configuration manager (stubbed)
- [ ] FreeBSD interface (stubbed)
- [ ] CLI with readline
- [ ] Main application with getopt
- [ ] Build system (CMake)
- [ ] Utilities (string/validation)

---

## Future Enhancements

- [ ] Actual FreeBSD network integration using ioctls (SIOCAIFADDR, SIOCSIFADDR, etc.)
- [ ] Full NETCONF operation support (edit-config, copy-config, lock, unlock, commit)
- [ ] Configuration persistence (save/load from file)
- [ ] Remote configuration backend (NETCONF server implementation)
- [ ] BGP routing support
- [ ] OSPF routing support
- [ ] Firewall rules (pf integration)
- [ ] NAT configuration
- [ ] DHCP server/client
- [ ] DNS configuration
- [ ] Transaction support (commit/rollback with actual system changes)
- [ ] Remote management (SSH/API)
- [ ] Configuration validation before apply
- [ ] Dry-run mode
