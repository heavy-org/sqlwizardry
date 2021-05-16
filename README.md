# SQLWizardry

https://github.com/HeavyHat/sqlwizardry/workflows/Build/badge.svg

## Introduction

sqlwizardry is a C++ based ORM mechanism that prioritises compile-time vaalidation and compoability. This allows the developer to write statements such as:

```cpp

SQLWIZARDRY_TABLE(Position,
    "position",
    (Column<std::string>) position_id,
    (Column<std::string>) source,
    (Column<int>) flag
)
int main() {
auto engine = engine::PostgresSQL{"position_db"};
auto position = create<Position>("pos_1", "csv", 1);
auto result = Position::query(engine)
    .select(Position::position_id)
    .where(Position::position_id != "A" and Position::flag == 2 and !Position::source.in(""))
    .order_by(Position::position_id)
    .all();
result.begin();
}
```

The above would be translated to an SQL statement run against a  particular database of particular type. At compile time the framework would induce an assertion if a given statement does not make sense or a particular DB does not support the type of statement you are attempting to run. All with an extremely performant compile time framework and the ability to create database objects ready-to-go in memory.