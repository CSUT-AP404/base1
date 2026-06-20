# Banking System — Phase 1
**Course:** AP
**Group:** [Group Name]
**Members:** [Member 1] — [Member 2]

---

## Project Structure
```
├── main.cpp        # Entry point
├── src/            # Source files (.cpp)
├── include/        # Header files (.h)
├── data/           # JSON data files (git-ignored at runtime)
└── README.md
```

## Compile & Run
```bash
g++ -std=c++17 -o bank_system main.cpp src/*.cpp
./bank_system
```

## Features (Phase 1)
- Branch management: create and list branches
- Account management: create, close, delete, list accounts
- Transactions: deposit, withdraw, transfer
- History: get balance, transaction history, transaction details
- Data management: clear history, reset all
- Passwords hashed with SHA-256
- All data persisted in JSON files under data/

## Notes
- Data files are created automatically on first run
- If no data files exist, system starts with empty state
- Never commit your data/ JSON files (already in .gitignore)