# Signal Conversation Archive Backup

## SCAB

Welcome to [Signal Conversation Archive Backup (SCAB)](https://github.com/prj-2501/signal-backup)!

Full writeup is at: https://matt.sh/signal-backup

## Usage

To backup your Signal Desktop database, run the following commands to:

- check out SCAB
- install Python requirements
  - Before doing so, make sure you install the version `4.3.0` of `sqlcipher` (which is the version used by Signal-Desktop as of 12-01-2021).
  - `pysqlcipher` must be built and linked against the above mentioned version of `sqlcipher` otherwise you would get "file is encrypted or is not a database" error.
- copy your Signal Desktop database (and attachments) into a new directory so nothing is read against your live Signal DB
- generate a single local HTML page web viewer for all your conversations

```sh
git clone https://github.com/prj-2501/signal-backup
pip3 install -r requirements.txt
cd signal-backup
```

```sh
# For macos:
rsync -avz "/Users/$(whoami)/Library/Application Support/Signal" backup
```

```sh
# For Linux:
mkdir backup
rsync -avz ~/.config/Signal/sql backup/sql
rsync -avz ~/.config/Signal/config.json backup/config.json
```

```
python3 scab.py
firefox myConversations.html
```

## Acknowledgement

The original version was written by [mattsta](https://github.com/mattsta).
