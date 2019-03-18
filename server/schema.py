# Super simple database schema and migration script.
# Add a column, and any existing rows will have it set to its default value.
# Remove a column, and the data will disappear.
# Change a column, and... you'll have to update it manually.
from collections import defaultdict
import re
import os
import sys
import psycopg2

def main(dry=True):
  schema = open('schema.sql').read()
  statements = map(str.strip, re.split(r';\n', schema))
  kinds = [
    'DROP TABLE',
    'CREATE TABLE',
    'INDEX',
  ]

  statements_of_kind = defaultdict(list)
  for statement in statements:
    if not statement: continue
    for kind in kinds:
      if kind in statement:
        statements_of_kind[kind].append(statement)
        break
    else:
      print("Error: I don't know what this means. Stopping just in case.")
      print(statement)
      return

  conn = psycopg2.connect(os.environ['DB_CONNECTION_STRING'])
  c = conn.cursor()

  creates = []
  for statement in statements_of_kind['CREATE TABLE']:
    table_name = re.search(r'CREATE TABLE IF NOT EXISTS ([a-z_]+)', statement, re.DOTALL)[1]
    table_name_new = table_name + '_new'
    creates.append((table_name, table_name_new, statement))

  # Create dummy tables if needed
  for table_name, table_name_new, statement in creates:
    c.execute(statement)

  # Create new tables and copy data over.
  print('### Creating new tables.')
  for table_name, table_name_new, statement in creates:
    print(table_name)
    statement_new = re.sub(table_name, table_name_new, statement)
    c.execute(f'''
      DROP TABLE IF EXISTS {table_name_new}
    ''')
    c.execute(statement_new)
    c.execute('''
      SELECT column_name FROM information_schema.columns WHERE table_name = %s
    ''', (table_name,))
    old_columns = list(sum(c.fetchall(), ()))
    c.execute('''
      SELECT column_name FROM information_schema.columns WHERE table_name = %s
    ''', (table_name_new,))
    new_columns = list(sum(c.fetchall(), ()))
    shared_columns = list(set(old_columns) & set(new_columns))
    added_columns = list(set(new_columns) - set(old_columns))
    removed_columns = list(set(old_columns) - set(new_columns))
    if added_columns or removed_columns:
      if added_columns:
        print(f'Columns added: {added_columns}')
      if removed_columns:
        print(f'Columns removed: {removed_columns}')
    ported_columns = ', '.join(shared_columns)
    c.execute(f'''
      INSERT INTO {table_name_new} ({ported_columns})
      SELECT {ported_columns} FROM {table_name};
    ''')

  # Delete original tables and rename new ones.
  print('### Replacing original tables.')
  for table_name, table_name_new, statement in creates:
    c.execute(f'''
      DROP TABLE IF EXISTS {table_name} CASCADE;
      ALTER TABLE {table_name_new} RENAME TO {table_name};
    ''')

  print(f"### Creating {len(statements_of_kind['INDEX'])} indexes.")
  for statement in statements_of_kind['INDEX']:
    c.execute(statement)

  # jim: MUST update if you add any more autoincrementing columns
  print('### Syncing sequences.')
  for tbl in ['adrestia_notifications', 'adrestia_rules']:
    print(tbl)
    c.execute(f'''
      SELECT setval('{tbl}_new_id_seq', (SELECT MAX(id) FROM {tbl}));
      ALTER SEQUENCE {tbl}_new_id_seq RENAME TO {tbl}_id_seq;
    ''')

  print('### Done')
  if dry:
    print('Dry run seems to have worked.')
    print('Pass in "dewit" to change the database for real.')
  else:
    print('Committing.')
    conn.commit()

if __name__ == '__main__':
  main(dry=(len(sys.argv) < 2 or sys.argv[1] != 'dewit'))
