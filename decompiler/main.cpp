#include <object_file/object_file_db.h>

static decompiler::object_file_db ofdb;

int main()
{
  ofdb.extract_dgos("C:\\Users\\Michael\\Downloads\\jx\\game");
  ofdb.process_linked_data();
  ofdb.find_code();
  return 0;
}