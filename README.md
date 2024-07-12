<div align="center">
<p>CoreDB :zap:</p>
<p>CoreDB is a relational database created in C++ as part of the C++ Programming course. The project is undertaken to enhance programming skills and understand the fundamentals of relational databases.</p>
</div>

## Project Features
- **SQL-like Syntax:** CoreDB provides SQL-like syntax, allowing interaction with the database through structured query language queries.
- **Relational Data Structure:** The project implements a relational data structure, enabling the creation, modification, and deletion of tables and relationships between them.
- **Basic CRUD Operations:** CoreDB supports basic CRUD operations (Create, Read, Update, Delete), allowing efficient data management in the database.

## Usage Instructions
### Requirements
- A C++ compiler compatible with C++23 or later
- CMake

### Installation
1. Download the project source files
   - Clone the CoreDB repository from GitHub:
   ```bash
   git clone https://github.com/aimlesx/core-db.git
   ```
2. Compile the project
   - Navigate to the project directory:
   ```bash
   cd CoreDB
   ```
   - Create a directory for build files:
   ```bash
   mkdir build
   cd build
   ```
   - Generate build files using CMake:
   ```bash
   cmake ..
   ```
   - Compile the project:
   ```bash
   make
   ```
3. Run the project
   - After successful compilation, run the project:
   ```bash
   ./CoreDB
   ```
4. Interact with the database
   - Use the interface that appears after launching the program to perform operations on the database.

**Installation Notes:**
- Ensure all necessary tools and dependencies listed above are installed.
- The environment in which the project was developed and tested:
   - Windows 10
   - CMake 3.28.1
   - Clang 17.0.5

### Query Language Documentation
#### DDL (Data Definition Language)
- CREATE
- DROP
- ALTER
- TRUNCATE
- RENAME
#### DQL (Data Query Language)
- SELECT
#### DML (Data Manipulation Language)
- INSERT
- UPDATE
- DELETE
- STORE
- RESTORE
- EXPLAIN PLAN
#### DCL (Data Control Language)
- GRANT