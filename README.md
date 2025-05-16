# Graph Partitioning Tool

Program dzieli zadany graf na *k* części przy zadanym marginesie procentowym. Celem jest minimalizacja liczby przeciętych krawędzi oraz zachowanie równowagi liczby wierzchołków w każdej części (z dokładnością do podanego marginesu procentowego).

Domyślnie, jeśli użytkownik nie poda parametrów, graf dzielony jest na 2 partycje z marginesem 10%.

---

## 📁 Struktura projektu

```
.
├── graph.c / graph.h       # Funkcje do operacji na grafie i partycjonowania
├── input.c / input.h       # Obsługa wczytywania grafów (m.in. CSR)
├── main.c                  # Tryb podstawowego działania programu
├── test-file.c             # Tryb interaktywnych testów
├── Makefile                # Kompilacja programu (z opcjami: test, main)
├── graf.csrrg              # Przykładowy graf (18 wierzchołków)
├── graf3.csrrg             # Przykładowy graf (2054 wierzchołków)
├── graf6.csrrg             # Przykładowy graf (50 wierzchołków)
```

---

## ⚙️ Kompilacja

Można skompilować dwa warianty programu:

```bash
make test       # Buduje wersję testową (interaktywne opcje testów)
make main       # Buduje wersję główną z domyślnym testem lub wejściem z pliku
```

---

## ▶️ Uruchamianie

### 1. Wersja testowa:

```bash
./test.out
```

Pojawi się menu:

```
Welcome to test file, choose your test:
1 - Basic test 12 nodes, 3 columns and 4 rows, connected: up, down, right
2 - Creates irregular graph with 12 nodes
3 - Hybrid custom-random graph – choose number of nodes and connection probability
```

Następnie użytkownik podaje:
- liczbę partycji (`k`), np. 4,
- maksymalną dopuszczalną różnicę rozmiarów partycji jako wartość z przedziału (0, 1), np. `0.2` (czyli 20%).

### 2. Wersja główna:

```bash
./main-run.out          # Domyślnie uruchamia podstawowy test (12 wierzchołków)
./main-run.out -c graf.csrrg   # Wczytuje graf z pliku w formacie .csrrg
```

W obu przypadkach użytkownik proszony jest o podanie:
- liczby partycji `k`,
- dopuszczalnej różnicy rozmiarów partycji (precyzji).

---

## 🧠 Opis algorytmu

Algorytm opiera się na iteracyjnym podziale grafu metodą rozrostu partycji w oparciu o heurystykę DFS (Depth-First Search). Każda partycja rozrasta się z wybranego wierzchołka startowego, dodając kolejnych sąsiadów do momentu osiągnięcia limitu narzuconego przez `k` oraz margines błędu (`precision`).

### 🔸 Wybór wierzchołka startowego

Dla każdej nowej partycji wybierany jest wierzchołek o najwyższym **score**, obliczanym wg wzoru:

```
score = w1 * degUn + w2 * dens
```

Gdzie:
- `degUn` – liczba nieprzydzielonych sąsiadów wierzchołka,
- `dens` – liczba połączeń między tymi sąsiadami (czyli lokalna spójność),
- `w1` i `w2` to wagi (np. `w1 = 10`, `w2 = 1`).

Wierzchołki o `degUn <= 1` są pomijane jako źródła — są zbyt słabe do rozrostu.

### 🔸 Rozrost partycji (DFS)

Po wybraniu źródła, algorytm kontynuuje rozrost partycji przez dodawanie nieprzydzielonych sąsiadów o największym **score**. DFS jest ograniczony przez:
- docelowy rozmiar partycji wyliczony z `n / k`,
- oraz dozwolony margines błędu (precision) – np. przy `precision = 0.2` różnica między największą a najmniejszą partycją nie może przekroczyć 20%.

### 🔸 Heurystyka wyboru sąsiada

Podczas rozrostu partycji algorytm ocenia kandydatów do dodania na podstawie trzech czynników:

1. **Liczby połączeń wewnątrz aktualnej partycji** (*inner connections*),
2. **Liczby połączeń na zewnątrz partycji** (*outer connections*),
3. **Stopnia całkowitego wierzchołka** (czyli ile ma wszystkich sąsiadów).

Funkcja przypisuje punkty każdemu kandydatowi na podstawie wzoru:

```
score = w1 * innerConnections - w2 * outerConnections - w3 * degree + bonus
```

Gdzie:
- `w1`, `w2`, `w3` to odpowiednio: 2, 5, 1,
- `bonus = +300` jeśli sąsiad kończy „tunel” (ma tylko jedno połączenie i partycja osiągnęła minimalny rozmiar),
- `-50` jeśli tunelowy sąsiad jest przedwcześnie spotkany (partycja jeszcze zbyt mała),
- `+100` to przesunięcie w celu utrzymania dodatniego wyniku.

Dzięki temu:
- Preferowani są wierzchołki, które **utrzymują spójność partycji**,
- Unika się zbyt szybkiego zamykania partii przez dodawanie „tunelowych” końców,
- Rozrost minimalizuje wycieki na zewnątrz partycji.

Ta heurystyka jest bardziej zaawansowana niż prosta ocena `degUn + dens` i pozwala na **precyzyjne i kontrolowane budowanie partycji**.

---

## 📥 Dane wejściowe

Program przyjmuje grafy w formacie `.csrrg` (CSR – compressed sparse row). W repozytorium znajdują się trzy przykładowe pliki:

- `graf.csrrg` – 18 wierzchołków
- `graf6.csrrg` – 50 wierzchołków
- `graf3.csrrg` – 2054 wierzchołków

---

## 📤 Przykładowe wyjście programu

```
Partition sizes:
Partition 0 size = 3
Partition 1 size = 3
Partition 2 size = 3
Partition 3 size = 3

Partitions:
Partition 0:  0,  1,  4, 
Partition 1:  6,  9,  10, 
Partition 2:  5,  8,  11, 
Partition 3:  2,  3,  7, 

Partitions outer connections:
Partition 0 <-> 5
Partition 1 <-> 4
Partition 2 <-> 4
Partition 3 <-> 9
```

---

## 🔧 Wymagania

Brak specjalnych zależności. Do kompilacji potrzebny jest:
- Kompilator C (np. `gcc`)
- System zgodny z POSIX (Linux, macOS)

---

## 👥 Autor

- Michał Muszyński

---

## 📄 Licencja

Projekt edukacyjny – bez określonej licencji. Możesz korzystać, modyfikować i udostępniać z oznaczeniem źródła.
