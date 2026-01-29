# Rust CLI Project Structure

Standard source organization for Rust CLI binaries.

## Directory Layout

```
src/
├── main.rs              # Minimal entry point
├── lib.rs               # Library exports (optional)
├── cli/
│   ├── mod.rs           # CLI orchestrator with run()
│   ├── def.rs           # Clap argument definitions
│   └── action/          # Command handlers (multi-command CLIs)
│       ├── mod.rs       # Dispatcher
│       └── <cmd>.rs     # One file per command
└── <domain>/            # Domain-specific modules
```

## File Responsibilities

### `main.rs`

Minimal entry point. Delegates to `cli::run()`.

```rust
fn main() {
    if let Err(e) = my_crate::cli::run() {
        eprintln!("Error: {e}");
        std::process::exit(1);
    }
}
```

### `cli/mod.rs`

CLI orchestrator. Exports types, sets up environment, dispatches commands.

```rust
mod action;
mod def;

pub use def::{Args, Commands};

use clap::Parser;

pub fn run() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();
    
    // Setup (logging, colors, etc.)
    
    action::dispatch(args.command)
}
```

### `cli/def.rs`

Clap argument definitions only. No logic.

```rust
use clap::{Parser, Subcommand};

#[derive(Parser)]
#[command(name = "mycli", version, about)]
pub struct Args {
    /// Global flags
    #[arg(short, long)]
    pub verbose: bool,

    #[command(subcommand)]
    pub command: Commands,
}

#[derive(Subcommand)]
pub enum Commands {
    /// First command description
    Foo {
        #[arg(short, long)]
        option: Option<String>,
    },
    /// Second command description
    Bar {
        /// Positional argument
        target: String,
    },
}
```

### `cli/action/mod.rs`

Command dispatcher. Routes to handlers.

```rust
mod foo;
mod bar;

use super::Commands;

pub fn dispatch(cmd: Commands) -> Result<(), Box<dyn std::error::Error>> {
    match cmd {
        Commands::Foo { option } => foo::run(option),
        Commands::Bar { target } => bar::run(&target),
    }
}
```

### `cli/action/<cmd>.rs`

Individual command handler. Contains the command's logic.

```rust
pub fn run(option: Option<String>) -> Result<(), Box<dyn std::error::Error>> {
    // Command implementation
    Ok(())
}
```

## When to Use `action/` Directory

| CLI Type | Structure |
|----------|-----------|
| Single command | No `action/`, logic in `mod.rs` |
| 2-3 simple commands | Optional, can inline in `mod.rs` |
| 4+ commands | Use `action/` directory |
| Complex commands | Use `action/` directory |

## Optional Modules

Add as needed:

- `cli/log.rs` - Logging setup (fern, tracing, etc.)
- `cli/output.rs` - Output formatting utilities
- `cli/config.rs` - CLI-specific configuration

## See Also

- `cli-guidelines.md` - Interface, output formats, exit codes
- `coding-guidelines.md` - General coding standards
