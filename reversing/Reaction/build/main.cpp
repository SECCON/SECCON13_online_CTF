#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>
#include <unistd.h>

//#define DEBUG

constexpr ssize_t MOLECULE_SIZE = 2;

struct pair_hash {
  inline std::size_t operator()(const std::pair<ssize_t, ssize_t> & v) const {
    return (static_cast<size_t>(v.first) << 32 | static_cast<size_t>(v.second));
  }
};

enum Attribute {
  NONE, GREEN, RED, YELLOW, BLUE
};

class Atom {
public:
  Atom() : attr_(NONE) {}
  Atom(uint8_t attr) {
    switch (attr) {
      case 0: attr_ = GREEN; break;
      case 1: attr_ = RED; break;
      case 2: attr_ = YELLOW; break;
      case 3: attr_ = BLUE; break;
      default: attr_ = NONE; break;
    }
  }
  enum Attribute& attr() { return attr_; }
  char as_byte() { return static_cast<char>(attr_); }

private:
  enum Attribute attr_;
};


class Molecule {
public:
  Molecule(std::mt19937& rand) {
    int a = rand();
    for (auto& atom : atoms_)
      atom = Atom(rand() % 4);
  }

  Atom& atom(ssize_t i) { return atoms_.at(i); }

private:
  std::array<Atom, MOLECULE_SIZE> atoms_;
  uint8_t direction_;
};


struct Input {
  uint8_t offset;
  uint8_t direction;
};


class IO {
public:
  void out(Molecule& block) {
    uint8_t out[MOLECULE_SIZE];
    for (ssize_t i = 0; i < MOLECULE_SIZE; i++)
      out[i] = block.atom(i).as_byte();
    write(STDOUT_FILENO, out, sizeof(out));
  }

  Input in() {
    Input in;
    read(STDIN_FILENO, &in, sizeof(in));
    return std::move(in);
  }
};

class Environment {
public:
  Environment(ssize_t width, ssize_t height, uint32_t seed, IO *io) :
    width_(width), height_(height), rand_(seed), io_(io), max_chain_(0)
  {
    field_.resize(height);
    for (ssize_t i = 0; i < height; i++)
      field_[i].resize(width);
  }

  int chain() { return max_chain_; }

  bool set() {
    Input in;
    Molecule block(rand_);

    io_->out(block);
    in = io_->in();

    if ((in.direction > 3)
        || (in.direction % 2 == 0 && in.offset >= width_)
        || (in.direction % 2 == 1 && in.offset > width_ - MOLECULE_SIZE))
      return false;

    if (in.direction == 0)
      for (ssize_t i = 0; i < MOLECULE_SIZE; i++) {
        if (field_.at(height_-1-i).at(in.offset).attr() != NONE) return false;
        field_.at(height_-1-i).at(in.offset) = block.atom(i);
      }
    else if (in.direction == 1)
      for (ssize_t i = 0; i < MOLECULE_SIZE; i++) {
        if (field_.at(height_-1).at(in.offset+i).attr() != NONE) return false;
        field_.at(height_-1).at(in.offset+i) = block.atom(i);
      }
    else if (in.direction == 2)
      for (ssize_t i = 0; i < MOLECULE_SIZE; i++) {
        if (field_.at(height_-MOLECULE_SIZE+i).at(in.offset).attr() != NONE) return false;
        field_.at(height_-MOLECULE_SIZE+i).at(in.offset) = block.atom(i);
      }
    else if (in.direction == 3)
      for (ssize_t i = 0; i < MOLECULE_SIZE; i++) {
        if (field_.at(height_-1).at(in.offset+MOLECULE_SIZE-1-i).attr() != NONE) return false;
        field_.at(height_-1).at(in.offset+MOLECULE_SIZE-1-i) = block.atom(i);
      }

    return true;
  }

#ifdef DEBUG
  void debug_draw() {
    for (ssize_t y = height_ - 1; y >= 0; y--) {
      std::cout << "|";
      for (ssize_t x = 0; x < width_; x++) {
        char atom = ' ';
        switch (field_[y][x].attr()) {
          case RED: atom = 'R'; break;
          case BLUE: atom = 'B'; break;
          case YELLOW: atom = 'Y'; break;
          case GREEN: atom = 'G'; break;
        }
        std::cout << atom;
        //std::cout << (int)field_[y][x].as_byte();
      }
      std::cout << "|" << std::endl;
    }

    std::cout << "+";
    for (ssize_t x = 0; x < width_; x++) {
      std::cout << "-";
    }
    std::cout << "+" << std::endl;
  }
#endif

  bool fall() {
    bool changed = false;

    for (ssize_t x = 0; x < width_; x++) {
      ssize_t h;
      for (ssize_t y = 0; y < height_; y++) {
        h = y;
        if (field_[y][x].attr() == NONE)
          break;
      }

      for (ssize_t y = h + 1; y < height_; y++) {
        if (field_[y][x].attr() != NONE) {
          field_[h++][x].attr() = field_[y][x].attr();
          field_[y][x].attr() = NONE;
          changed = true;
        }
      }
    }

    return changed;
  }

  bool react(ssize_t x, ssize_t y, enum Attribute attr) {
    std::vector<std::pair<ssize_t, ssize_t>> stack;
    std::unordered_set<std::pair<ssize_t, ssize_t>, pair_hash> marked;

    stack.emplace_back(std::make_pair(x, y));

    size_t count = 0;
    while (stack.size()) {
      std::pair<ssize_t, ssize_t> xy = stack.back();
      ssize_t x = xy.first;
      ssize_t y = xy.second;
      stack.pop_back();

      auto res = std::find_if(marked.begin(), marked.end(), [x, y](auto& v) {
        return v.first == x && v.second == y;
      });
      if (res != marked.end())
        continue;

      marked.insert(std::make_pair(x, y));
      count++;

      if (x > 0 && field_.at(y).at(x-1).attr() == attr)
        stack.emplace_back(std::make_pair(x-1, y));
      if (y > 0 && field_.at(y-1).at(x).attr() == attr)
        stack.emplace_back(std::make_pair(x, y-1));
      if (x < width_-1 && field_.at(y).at(x+1).attr() == attr)
        stack.emplace_back(std::make_pair(x+1, y));
      if (y < height_-1 && field_.at(y+1).at(x).attr() == attr)
        stack.emplace_back(std::make_pair(x, y+1));
    }

    if (count >= 4) {
      for (auto& xy: marked)
        field_.at(xy.second).at(xy.first).attr() = NONE;
      return true;
    }

    return false;
  }

  bool reduce(int& chain) {
    for (ssize_t y = height_ - 1; y >= 0; y--) {
      for (ssize_t x = 0; x < width_; x++) {
        if (field_[y][x].attr() != NONE) {
          auto attr = field_[y][x].attr();
          if (react(x, y, attr)) {
            chain++;
            return true;
          }
        }
      }
    }
    return false;
  }

  bool update() {
    int chain;

    if (!set())
      return false;

    bool changed;

    chain = 0;
    do {
      changed = false;
      changed |= fall();
      changed |= reduce(chain);
    } while (changed);

#ifdef DEBUG
    debug_draw();
#endif
    if (chain > max_chain_)
      max_chain_ = chain;

    return true;
  }

private:
  ssize_t width_, height_;
  int max_chain_;
  std::vector<std::vector<Atom>> field_;
  std::mt19937 rand_;
  IO *io_;
};

int main() {
  std::string flag;
  std::ifstream f("flag.txt");
  if (!f) {
    std::cerr << "Flag not found" << std::endl;
    return 1;
  }
  getline(f, flag);
  f.close();

  uint32_t seed = 1;
  for (size_t i = 0; i < flag.size(); i++)
    seed *= static_cast<uint32_t>(flag[i]);

  IO io;
  Environment stage(14, 14, seed, &io);

  while (stage.update());

  if (stage.chain() >= 14) {
    std::cout << "Correct!" << std::endl
              << "FLAG: " << flag << std::endl;
  } else {
    std::cout << "Wrong..." << std::endl;
  }

  return 0;
}
