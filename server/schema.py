# Super simple database schema and migration script.
# Add a column, and any existing rows will have it set to its default value.
# Remove a column, and the data will disappear.
# Change a column, and... you'll have to update it manually.
from collections import defaultdict
import re
import os
import psycopg2

def main():
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

  # Lock! Down! Everything!
  print('Taking locks.')
  for table_name, table_name_new, statement in creates:
    c.execute(f'''
      LOCK TABLE {table_name} IN EXCLUSIVE MODE
    ''')

  # Create new tables and copy data over.
  to_replace = set()
  print('Creating new tables.')
  for table_name, table_name_new, statement in creates:
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
      print(table_name)
      if added_columns:
        print(f'Columns added: {added_columns}')
      if removed_columns:
        print(f'Columns removed: {removed_columns}')
      ported_columns = ', '.join(shared_columns)
      c.execute(f'''
        INSERT INTO {table_name_new} ({ported_columns})
        SELECT {ported_columns} FROM {table_name};
      ''')
      to_replace.add(table_name)
    else:
      print(f'{table_name}: Leaving alone.')
      c.execute(f'''
        DROP TABLE {table_name_new} CASCADE
      ''')

  # Delete original tables and rename new ones.
  print('Deleting original tables.')
  for table_name, table_name_new, statement in creates:
    if table_name not in to_replace: continue
    c.execute(f'''
      DROP TABLE IF EXISTS {table_name} CASCADE;
      ALTER TABLE {table_name_new} RENAME TO {table_name};
    ''')
  conn.commit()

  print('Creating indices.')
  for statement in statements_of_kind['INDEX']:
    print(statement)
    c.execute(statement)

if __name__ == '__main__':
  main()
