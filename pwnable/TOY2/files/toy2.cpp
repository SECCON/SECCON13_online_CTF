#include <array>
#include <cstdint>
#include <iostream>
#include <span>
#include <stdexcept>
#include <utility>

#define MEM_SIZE (1U<<12)
#define MAX_RUN  0x10000

using OpCode = uint8_t;
using Reg    = uint16_t;
using Addr   = uint16_t;
using Inst   = std::pair<OpCode, Addr>;

class VM {
public:
  VM() : _mem(_data.data(), _data.size()) {
    _data.fill(0);
    reset_registers();
  }

  uint8_t& at(Addr addr) {
    return _mem[addr];
  }

  uint16_t mem_read(Addr addr) {
    return at(addr) | (at(addr+1) << 8);
  }

  void mem_write(Addr addr, uint16_t value) {
    at(addr)   = value & 0xff;
    at(addr+1) = value >> 8;
  }

  inline uint16_t size() {
    return _mem.size();
  }

  inline void validate_src(Addr addr) {
    if (addr > size() - 2)
      throw std::out_of_range("Address out of range (read)");
  }

  inline void validate_dest(Addr addr) {
    if (addr > size() - 2)
      throw std::out_of_range("Address out of range (write)");
  }

  inline void validate_vec(Addr addr) {
    validate_src(addr);
    if (mem_read(addr) & 1)
      throw std::invalid_argument("Unaligned jump address");
  }

  virtual void reset_registers() {
    _regs.pc = 0;
    _regs.a  = 0;
    _regs.t  = 0;
    _regs.c  = false;
    _regs.z  = false;
  }

  virtual void dump_registers() {
    std::cout << "A = 0x" << std::hex << _regs.a << std::endl
              << "T = 0x" << std::hex << _regs.t << std::endl;
  }

  Inst fetch_and_decode() {
    uint16_t v = mem_read(_regs.pc);
    _regs.pc += 2;
    return std::make_pair(v >> 12, v & 0xfff);
  }

  void execute_and_store(Inst &inst) {
    OpCode op   = inst.first;
    Addr   addr = inst.second;
    bool c;

    switch (op) {
      case 0: /* JMP vec */
        validate_vec(addr);
        _regs.pc = mem_read(addr);
        break;

      case 1: /* ADC src */
        validate_src(addr);
        c = __builtin_add_overflow(_regs.a, mem_read(addr), &_regs.a);
        _regs.c = c | __builtin_add_overflow(_regs.a, _regs.c, &_regs.a);
        _regs.z = _regs.a == 0;
        break;

      case 2: /* XOR src */
        validate_src(addr);
        _regs.a ^= mem_read(addr);
        _regs.z = _regs.a == 0;
        break;

      case 3: /* SBC src */
        validate_src(addr);
        c = __builtin_sub_overflow(_regs.a, mem_read(addr), &_regs.a);
        _regs.c = c | __builtin_sub_overflow(_regs.a, _regs.c, &_regs.a);
        _regs.z = _regs.a == 0;
        break;

      case 4: /* ROR */
        c = _regs.a >> 15;
        _regs.a = (_regs.a << 1) | _regs.c;
        _regs.c = c;
        _regs.z = _regs.a == 0;
        break;

      case 5: /* TAT */
        _regs.t = _regs.a;
        break;

      case 6: /* OR src */
        validate_src(addr);
        _regs.a |= mem_read(addr);
        _regs.z = _regs.a == 0;
        break;

      case 7: /* illegal */
        throw std::runtime_error("Illegal instruction");

      case 8: /* AND src */
        validate_src(addr);
        _regs.a &= mem_read(addr);
        _regs.z = _regs.a == 0;
        break;

      case 9: /* LDC src */
        validate_src(addr);
        _regs.a = mem_read(addr);
        _regs.c = false;
        break;

      case 10: /* BCC vec */
        validate_vec(addr);
        if (_regs.c) _regs.pc = mem_read(addr);
        break;

      case 11: /* BNE vec */
        validate_vec(addr);
        if (_regs.z) _regs.pc = mem_read(addr);
        break;

      case 12: /* LDI */
        _regs.a = mem_read(_regs.a & (size() - 1));
        break;

      case 13: /* STT */
        mem_write(_regs.a & (size() - 1), _regs.t);
        break;

      case 14: /* LDA src */
        validate_src(addr);
        _regs.a = mem_read(addr);
        break;

      case 15: /* STA dest */
        validate_dest(addr);
        mem_write(addr, _regs.a);
        break;
    }
  }

  void run(Addr pc) {
    _regs.pc = pc;

    for (size_t i = 0; i < MAX_RUN && _regs.pc < size(); i++) {
      Inst inst = fetch_and_decode();
      execute_and_store(inst);
    }
  }

private:
  std::array<uint8_t, MEM_SIZE> _data;
  std::span<uint8_t> _mem;
  struct {
    Reg pc;
    Reg a;
    Reg t;
    bool c;
    bool z;
  } _regs;
};

int main() {
  VM *vm = new VM();
  std::setbuf(stdin, NULL);
  std::setbuf(stdout, NULL);

  for (Addr i = 0; i < MEM_SIZE; i++)
    if (fread(&vm->at(i), 1, 1, stdin) <= 0)
      break;

  std::cout << "[+] Running..." << std::endl;
  try {
    vm->run(0);
  } catch (const std::exception &e) {
    std::cout << "[-] Error: " << e.what() << std::endl;
  }
  std::cout << "[+] Done." << std::endl;
  vm->dump_registers();

  delete vm;
  return 0;
}
