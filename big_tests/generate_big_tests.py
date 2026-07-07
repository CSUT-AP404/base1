from pathlib import Path


OUT_DIR = Path(__file__).resolve().parent
ACCOUNT_START = 5022374188200000


def national_code(index: int) -> str:
    """Return a valid 10-digit Iranian national code for generated tests."""
    first9 = f"{100000000 + index:09d}"[-9:]
    total = sum(int(digit) * weight for digit, weight in zip(first9, range(10, 1, -1)))
    remainder = total % 11
    check = remainder if remainder < 2 else 11 - remainder
    return first9 + str(check)


def account_number(sequence: int) -> str:
    raw = f"{ACCOUNT_START + sequence:016d}"
    return "-".join(raw[i : i + 4] for i in range(0, 16, 4))


def write_lines(name: str, lines: list[str]) -> None:
    path = OUT_DIR / name
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"{path.name}: {len(lines)} lines")


def add_user_with_accounts(
    lines: list[str],
    user_index: int,
    account_count: int,
    account_sequence: int,
) -> int:
    code = national_code(user_index)
    user_pass = f"upass{user_index:06d}"
    lines += ["signup", code, user_pass, "login", code, user_pass]
    for _ in range(account_count):
        account_sequence += 1
        lines += ["open_account", f"apass{account_sequence:07d}"]
    lines.append("logout")
    return account_sequence


def test_01_signup_open_accounts() -> None:
    lines: list[str] = []
    account_sequence = 0
    for user_index in range(1, 5001):
        account_sequence = add_user_with_accounts(lines, user_index, 3, account_sequence)
    write_lines("big_01_signup_open_accounts.in", lines)


def test_02_public_deposits() -> None:
    lines: list[str] = []
    account_sequence = 0
    for user_index in range(10001, 12001):
        account_sequence = add_user_with_accounts(lines, user_index, 2, account_sequence)

    total_accounts = account_sequence
    for round_index in range(4):
        for account_sequence in range(1, total_accounts + 1):
            amount = 1000 + ((account_sequence + round_index) % 97)
            lines.append(f"deposit_to {account_number(account_sequence)} {amount}")

    write_lines("big_02_public_deposits.in", lines)


def test_03_transfers_balance_withdrawals() -> None:
    lines: list[str] = []
    account_sequence = 0
    first_account_by_user: dict[int, int] = {}
    second_account_by_user: dict[int, int] = {}

    for user_index in range(20001, 21201):
        first_seq = account_sequence + 1
        account_sequence = add_user_with_accounts(lines, user_index, 2, account_sequence)
        first_account_by_user[user_index] = first_seq
        second_account_by_user[user_index] = first_seq + 1

    for seq in range(1, account_sequence + 1):
        lines.append(f"deposit_to {account_number(seq)} 1000000")

    users = list(first_account_by_user)
    for offset, user_index in enumerate(users):
        code = national_code(user_index)
        user_pass = f"upass{user_index:06d}"
        src_seq = first_account_by_user[user_index]
        dst_user = users[(offset + 1) % len(users)]
        dst_seq = second_account_by_user[dst_user]
        lines += [
            "login",
            code,
            user_pass,
            f"send_money {account_number(src_seq)} {account_number(dst_seq)} 25000",
            f"apass{src_seq:07d}",
            f"balance_inquiry {account_number(src_seq)}",
            f"withdraw_from {account_number(src_seq)} 10000",
            f"apass{src_seq:07d}",
            "logout",
        ]

    write_lines("big_03_transfers_balance_withdrawals.in", lines)


def test_04_error_paths() -> None:
    lines: list[str] = []
    account_sequence = 0
    for user_index in range(30001, 30601):
        account_sequence = add_user_with_accounts(lines, user_index, 1, account_sequence)
        lines.append(f"deposit_to {account_number(account_sequence)} 5000")

    missing_account = "9999-9999-9999-9999"
    for local_index, user_index in enumerate(range(30001, 30601), start=1):
        code = national_code(user_index)
        user_pass = f"upass{user_index:06d}"
        own_seq = local_index
        other_seq = 1 if local_index != 1 else 2
        lines += [
            "login",
            code,
            "wrong-password",
            "login",
            code,
            user_pass,
            f"withdraw_from {account_number(own_seq)} 100",
            "wrong-account-password",
            f"withdraw_from {account_number(other_seq)} 100",
            f"send_money {account_number(own_seq)} {missing_account} 100",
            f"balance_inquiry {missing_account}",
            "list_accounts",
            "logout",
            "logout",
        ]

    write_lines("big_04_error_paths.in", lines)


def test_05_delete_cleanup() -> None:
    lines: list[str] = []
    account_sequence = 0
    for user_index in range(40001, 41501):
        account_sequence = add_user_with_accounts(lines, user_index, 1, account_sequence)

    for seq in range(1, account_sequence + 1):
        lines.append(f"deposit_to {account_number(seq)} 1000")

    for local_index, user_index in enumerate(range(40001, 41501), start=1):
        code = national_code(user_index)
        user_pass = f"upass{user_index:06d}"
        lines += [
            "login",
            code,
            user_pass,
            f"delete_my_account {account_number(local_index)}",
            f"apass{local_index:07d}",
            f"withdraw_from {account_number(local_index)} 1000",
            f"apass{local_index:07d}",
            f"delete_my_account {account_number(local_index)}",
            f"apass{local_index:07d}",
            "delete_my_user",
            user_pass,
        ]

    write_lines("big_05_delete_cleanup.in", lines)


def write_readme() -> None:
    readme = """# Big Phase 2 Tests

These are large input workloads for `user.cpp`. They are intended mostly for
memory-usage experiments, persistence stress, and broad behavioral smoke tests.

Run each file on a fresh data directory/state unless you intentionally want a
combined persistence run.

Important assumption: the generated account numbers are deterministic by account
creation order. The first opened account is assumed to be:

```text
5022-3741-8820-0001
```

The second opened account is `5022-3741-8820-0002`, and so on. If your reference
solution uses a different account-number generator, adjust `ACCOUNT_START` in
`generate_big_tests.py` and regenerate the files.

Suggested memory run:

```bash
/usr/bin/time -v ./user < big_01_signup_open_accounts.in > big_01.out
```

For the bonus report, record at least the test file name, the command used, and
the reported maximum resident set size.

## Files

- `big_01_signup_open_accounts.in`: many users, logins, and account creation.
- `big_02_public_deposits.in`: many public deposits to many accounts.
- `big_03_transfers_balance_withdrawals.in`: transfers, balance inquiries, and withdrawals.
- `big_04_error_paths.in`: wrong passwords, missing accounts, invalid commands, and ownership errors.
- `big_05_delete_cleanup.in`: positive-balance delete failures followed by cleanup.
"""
    (OUT_DIR / "README.md").write_text(readme, encoding="utf-8")


def main() -> None:
    test_01_signup_open_accounts()
    test_02_public_deposits()
    test_03_transfers_balance_withdrawals()
    test_04_error_paths()
    test_05_delete_cleanup()
    write_readme()


if __name__ == "__main__":
    main()
