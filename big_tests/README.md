# Big Phase 2 Tests

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
