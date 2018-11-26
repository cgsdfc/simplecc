
#include "symtable.h"
#include <cassert>

void TestSymbolTable(SymbolTable &symtable) {
  auto &global = symtable.global;
  auto &locals = symtable.locals;

  {
    assert(global.size() == 29);

    {
      assert(global.count("positive"));
      const SymbolEntry &e = global.at("positive");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "positive");
    }

    {
      assert(global.count("negative"));
      const SymbolEntry &e = global.at("negative");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "negative");
    }

    {
      assert(global.count("max_heap_size"));
      const SymbolEntry &e = global.at("max_heap_size");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "max_heap_size");
    }

    {
      assert(global.count("character"));
      const SymbolEntry &e = global.at("character");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "character");
    }

    {
      assert(global.count("is_valid"));
      const SymbolEntry &e = global.at("is_valid");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "is_valid");
    }

    {
      assert(global.count("hasattr"));
      const SymbolEntry &e = global.at("hasattr");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "hasattr");
    }

    {
      assert(global.count("digit"));
      const SymbolEntry &e = global.at("digit");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "digit");
    }

    {
      assert(global.count("letter"));
      const SymbolEntry &e = global.at("letter");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "letter");
    }

    {
      assert(global.count("heap"));
      const SymbolEntry &e = global.at("heap");
      assert(e.IsArray());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heap");
    }

    {
      assert(global.count("index"));
      const SymbolEntry &e = global.at("index");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "index");
    }

    {
      assert(global.count("flag"));
      const SymbolEntry &e = global.at("flag");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "flag");
    }

    {
      assert(global.count("array"));
      const SymbolEntry &e = global.at("array");
      assert(e.IsArray());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "array");
    }

    {
      assert(global.count("module"));
      const SymbolEntry &e = global.at("module");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "module");
    }

    {
      assert(global.count("class"));
      const SymbolEntry &e = global.at("class");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "class");
    }

    {
      assert(global.count("errors"));
      const SymbolEntry &e = global.at("errors");
      assert(e.IsArray());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "errors");
    }

    {
      assert(global.count("input"));
      const SymbolEntry &e = global.at("input");
      assert(e.IsArray());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "input");
    }

    {
      assert(global.count("addi"));
      const SymbolEntry &e = global.at("addi");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "addi");
    }

    {
      assert(global.count("addc"));
      const SymbolEntry &e = global.at("addc");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "addc");
    }

    {
      assert(global.count("print_char"));
      const SymbolEntry &e = global.at("print_char");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "print_char");
    }

    {
      assert(global.count("print_string"));
      const SymbolEntry &e = global.at("print_string");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "print_string");
    }

    {
      assert(global.count("print_string_ex"));
      const SymbolEntry &e = global.at("print_string_ex");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "print_string_ex");
    }

    {
      assert(global.count("dead_loop"));
      const SymbolEntry &e = global.at("dead_loop");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "dead_loop");
    }

    {
      assert(global.count("prompt_int"));
      const SymbolEntry &e = global.at("prompt_int");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "prompt_int");
    }

    {
      assert(global.count("swap"));
      const SymbolEntry &e = global.at("swap");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "swap");
    }

    {
      assert(global.count("heapify"));
      const SymbolEntry &e = global.at("heapify");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heapify");
    }

    {
      assert(global.count("heap_sort"));
      const SymbolEntry &e = global.at("heap_sort");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heap_sort");
    }

    {
      assert(global.count("print_heap"));
      const SymbolEntry &e = global.at("print_heap");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "print_heap");
    }

    {
      assert(global.count("reset_heap"));
      const SymbolEntry &e = global.at("reset_heap");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "reset_heap");
    }

    {
      assert(global.count("main"));
      const SymbolEntry &e = global.at("main");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "main");
    }
  }

  assert(locals.find("addi") != locals.end());
  {
    auto &local = locals["addi"];
    assert(local.size() == 2);

    {
      assert(local.count("a"));
      const SymbolEntry &e = local.at("a");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "a");
    }

    {
      assert(local.count("b"));
      const SymbolEntry &e = local.at("b");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "b");
    }
  }

  assert(locals.find("addc") != locals.end());
  {
    auto &local = locals["addc"];
    assert(local.size() == 2);

    {
      assert(local.count("a"));
      const SymbolEntry &e = local.at("a");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "a");
    }

    {
      assert(local.count("b"));
      const SymbolEntry &e = local.at("b");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "b");
    }
  }

  assert(locals.find("print_char") != locals.end());
  {
    auto &local = locals["print_char"];
    assert(local.size() == 1);

    {
      assert(local.count("c"));
      const SymbolEntry &e = local.at("c");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "c");
    }
  }

  assert(locals.find("print_string") != locals.end());
  {
    auto &local = locals["print_string"];
    assert(local.size() == 0);
  }

  assert(locals.find("print_string_ex") != locals.end());
  {
    auto &local = locals["print_string_ex"];
    assert(local.size() == 1);

    {
      assert(local.count("expr"));
      const SymbolEntry &e = local.at("expr");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "expr");
    }
  }

  assert(locals.find("dead_loop") != locals.end());
  {
    auto &local = locals["dead_loop"];
    assert(local.size() == 0);
  }

  assert(locals.find("prompt_int") != locals.end());
  {
    auto &local = locals["prompt_int"];
    assert(local.size() == 1);

    {
      assert(local.count("val"));
      const SymbolEntry &e = local.at("val");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "val");
    }
  }

  assert(locals.find("swap") != locals.end());
  {
    auto &local = locals["swap"];
    assert(local.size() == 4);

    {
      assert(local.count("index1"));
      const SymbolEntry &e = local.at("index1");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "index1");
    }

    {
      assert(local.count("index2"));
      const SymbolEntry &e = local.at("index2");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "index2");
    }

    {
      assert(local.count("temp"));
      const SymbolEntry &e = local.at("temp");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "temp");
    }

    {
      assert(local.count("heap"));
      const SymbolEntry &e = local.at("heap");
      assert(e.IsArray());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heap");
    }
  }

  assert(locals.find("heapify") != locals.end());
  {
    auto &local = locals["heapify"];
    assert(local.size() == 8);

    {
      assert(local.count("index"));
      const SymbolEntry &e = local.at("index");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "index");
    }

    {
      assert(local.count("heap_size"));
      const SymbolEntry &e = local.at("heap_size");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "heap_size");
    }

    {
      assert(local.count("largest"));
      const SymbolEntry &e = local.at("largest");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "largest");
    }

    {
      assert(local.count("left"));
      const SymbolEntry &e = local.at("left");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "left");
    }

    {
      assert(local.count("right"));
      const SymbolEntry &e = local.at("right");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "right");
    }

    {
      assert(local.count("heap"));
      const SymbolEntry &e = local.at("heap");
      assert(e.IsArray());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heap");
    }

    {
      assert(local.count("swap"));
      const SymbolEntry &e = local.at("swap");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "swap");
    }

    {
      assert(local.count("heapify"));
      const SymbolEntry &e = local.at("heapify");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heapify");
    }
  }

  assert(locals.find("heap_sort") != locals.end());
  {
    auto &local = locals["heap_sort"];
    assert(local.size() == 4);

    {
      assert(local.count("heap_size"));
      const SymbolEntry &e = local.at("heap_size");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "heap_size");
    }

    {
      assert(local.count("i"));
      const SymbolEntry &e = local.at("i");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "i");
    }

    {
      assert(local.count("heapify"));
      const SymbolEntry &e = local.at("heapify");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heapify");
    }

    {
      assert(local.count("swap"));
      const SymbolEntry &e = local.at("swap");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "swap");
    }
  }

  assert(locals.find("print_heap") != locals.end());
  {
    auto &local = locals["print_heap"];
    assert(local.size() == 3);

    {
      assert(local.count("heap_size"));
      const SymbolEntry &e = local.at("heap_size");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "heap_size");
    }

    {
      assert(local.count("i"));
      const SymbolEntry &e = local.at("i");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "i");
    }

    {
      assert(local.count("heap"));
      const SymbolEntry &e = local.at("heap");
      assert(e.IsArray());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heap");
    }
  }

  assert(locals.find("reset_heap") != locals.end());
  {
    auto &local = locals["reset_heap"];
    assert(local.size() == 3);

    {
      assert(local.count("i"));
      const SymbolEntry &e = local.at("i");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "i");
    }

    {
      assert(local.count("max_heap_size"));
      const SymbolEntry &e = local.at("max_heap_size");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "max_heap_size");
    }

    {
      assert(local.count("heap"));
      const SymbolEntry &e = local.at("heap");
      assert(e.IsArray());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heap");
    }
  }

  assert(locals.find("main") != locals.end());
  {
    auto &local = locals["main"];
    assert(local.size() == 10);

    {
      assert(local.count("n"));
      const SymbolEntry &e = local.at("n");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "n");
    }

    {
      assert(local.count("temp"));
      const SymbolEntry &e = local.at("temp");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "temp");
    }

    {
      assert(local.count("i"));
      const SymbolEntry &e = local.at("i");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "i");
    }

    {
      assert(local.count("j"));
      const SymbolEntry &e = local.at("j");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "j");
    }

    {
      assert(local.count("array_size"));
      const SymbolEntry &e = local.at("array_size");
      assert(e.IsVariable());
      assert(e.GetScope() == Scope::Local);
      assert(e.GetName() == "array_size");
    }

    {
      assert(local.count("reset_heap"));
      const SymbolEntry &e = local.at("reset_heap");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "reset_heap");
    }

    {
      assert(local.count("max_heap_size"));
      const SymbolEntry &e = local.at("max_heap_size");
      assert(e.IsConstant());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "max_heap_size");
    }

    {
      assert(local.count("heap"));
      const SymbolEntry &e = local.at("heap");
      assert(e.IsArray());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heap");
    }

    {
      assert(local.count("heap_sort"));
      const SymbolEntry &e = local.at("heap_sort");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "heap_sort");
    }

    {
      assert(local.count("print_heap"));
      const SymbolEntry &e = local.at("print_heap");
      assert(e.IsFunction());
      assert(e.GetScope() == Scope::Global);
      assert(e.GetName() == "print_heap");
    }
  }
}
