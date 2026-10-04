# Route Finder in C

A command-line route finder built in C using real-world OpenStreetMap data. The program resolves addresses, places or coordinates, maps them to nearby street segments and uses breadth-first search to produce turn-by-turn directions.

The project began as a Data Structures and Algorithms university assignment and was developed collaboratively by Xavier Montaner, Albert Gummà and Víctor Murillo. This repository contains a cleaned and reorganized public edition prepared by Xavier Montaner.

## Highlights

- Linked lists for houses, places and street segments
- Fuzzy name matching using Levenshtein distance
- Haversine distance for geographic coordinates
- Street graph represented with a hash map of intersections
- Breadth-first search with a hash-based visited set
- Turn-by-turn route instructions
- Automated unit tests and GitHub Actions CI
- Performance comparison between sequential and hash-based graph lookup

## Build

You need a C11-compatible compiler such as GCC or Clang.

```bash
make
```

Or compile directly:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror \
  src/main.c src/houses.c src/pathfinding.c src/places.c src/streets.c \
  -o route-finder -lm
```

## Run

```bash
make run
```

The repository includes two compact demonstration maps: `xs_1` and `xs_2`.

## Tests

```bash
make test
```

## Project structure

```text
route-finder-c/
|-- src/          C implementation and headers
|-- tests/        unit tests
|-- maps/         compact demonstration datasets
|-- docs/         performance analysis and charts
|-- Makefile
`-- README.md
```

## Performance analysis

The original project compared sequential intersection lookup with a hash-map implementation and evaluated both approaches during BFS. See [the performance analysis](docs/performance-analysis.md) for the results and limitations.

## Data attribution

The included geographic datasets are derived from [OpenStreetMap](https://www.openstreetmap.org/) data. © OpenStreetMap contributors, available under the [Open Database License](https://opendatacommons.org/licenses/odbl/).

The datasets are included only as small demonstrations. Larger maps can be generated independently from OpenStreetMap exports.

## Project origin and attribution

Original university project authors:

- Xavier Montaner
- Albert Gummà
- Víctor Murillo

The assignment specification and initial project scaffolding were provided as part of the Universitat Pompeu Fabra Data Structures and Algorithms course. They are not included in this revised repository. The public edition reorganizes the codebase, removes course administration material and compiled artifacts, improves safety checks and adds standalone documentation.

## License

No software license is currently granted. The source is publicly visible for educational and portfolio purposes. The OpenStreetMap-derived datasets remain subject to the ODbL.