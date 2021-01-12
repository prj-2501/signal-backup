#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <list>
#include <cassert>
#include <sqlcipher/sqlite3.h>

using namespace std;

static int
noop_cb(void *NotUsed, int argc, char **argv, char **azColName)
{
  return 0;
}

static int
json_callback(void *mp_ptr, int argc, char **argv, char **azColName)
{
  auto p = static_cast<unordered_map<string,list<string>>*>(mp_ptr);
  assert(p);
  unordered_map<string,list<string>>& cid2json = *p;
  assert(argc == 2);
  if (argv[0] && argv[1]) {
    cid2json[string(argv[0])].push_back(string(argv[1]));
  }
  return 0;
}

void
dump_map_as_json(unordered_map<string,list<string>> const& cid2json, ofstream& fo)
{
  fo << "{\n";
  bool first = true;
  for (auto const& p : cid2json) {
    if (!first) fo << ",\n";
    first = false;

    fo << '"' << p.first << '"' << ": [\n";
    bool is_first = true;
    for (auto const& s : p.second) {
      if (!is_first) fo << ",\n";
      is_first = false;

      fo << s;;
    }
    fo << "\n]";
  }
  fo << "\n}";
}

bool
write_json_messages_to_file(sqlite3* db, string const& fname)
{
  char *errmsg = nullptr;
  unordered_map<string,list<string>> cid2json;
  int rc = sqlite3_exec(db, "SELECT conversationId, json FROM messages ORDER BY sent_at", json_callback, &cid2json, &errmsg);
  if (rc != SQLITE_OK) {
    cerr << "SQL error: " << errmsg << endl;
    sqlite3_free(errmsg);
    return false;
  }
  cout << cid2json.size() << " conversation record(s) found.";
  ofstream fo(fname, ofstream::out|ofstream::trunc);
  if (!fo) {
    cerr << "Could not open file '" << fname << "' for writing json data\n";
    return false;
  }
  dump_map_as_json(cid2json, fo);
  return true;
}

bool
decrypt_db(sqlite3* db, string const& key)
{
  char* errmsg = nullptr;
  ostringstream ss;
  ss << "PRAGMA KEY = \"x'" << key << "'\"";
  int rc = sqlite3_exec(db, ss.str().c_str(), noop_cb, 0, &errmsg);
  if (rc != SQLITE_OK) {
    cerr << "SQL error: " << errmsg << endl;
    sqlite3_free(errmsg);
    return false;
  }
  return true;
}

int main(int argc, char **argv){
  if (argc != 3) {
    fprintf(stderr, "Usage: %s DATABASE KEY\n", argv[0]);
    return 1;
  }

  sqlite3 *db;
  int rc = sqlite3_open(argv[1], &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  bool ret = decrypt_db(db, argv[2]);
  if (!ret) {
    sqlite3_close(db);
    return 1;
  }

  ret = write_json_messages_to_file(db, "messages.json");
  if (!ret) {
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);
  return 0;
}
