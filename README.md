# C Database
A Database written entirely in C.

## Authors
* Giada Amaducci
* [Anna Fabris](https://github.com/annafabris)
* Riccardo Lucchi

## About the Project
### Data storage
The project simulates the management of a Database using text files (.txt) for data storage.
Each table of the Database will be stored in a different text file.

### Input Commands
- The management of each table of the Database will be done using commands in a language provided in the project specifications, which echoes the SQL syntax.
    - `CREATE TABLE table_name (column1_name,…)` 
    - `INSERT INTO table_name (column1_name,…) VALUES (value1,…)` 
    - `CREATE TABLE table_name (column1_name,…)` 
    - `SELECT * FROM table_name` 
    - `SELECT column1_name,… FROM table_name` 
    - `SELECT column1_name,… FROM table_name WHERE condition`, where condition can be `==`, `>`, `>=`, `<` or `<=`
    - `SELECT column1_name,… FROM table_name ORDER BY column2_name ASC`, instead of `ASC`, `DESC` can be used
    - `SELECT column1_name FROM table_name GROUP BY column1_name`
### Output
To represent the Database in the text files (output) the output is provided in the following way.
```c 
TABLE table_name COLUMNS column1_name,..; 
ROW value1,...;
ROW value2,...;
```