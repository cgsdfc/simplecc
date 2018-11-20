
#include "symtable.h"
#include <cassert>

void TestSymbolTable(SymbolTable &symtable) {
  auto &global = symtable.global;
  auto &locals = symtable.locals;

  {
    assert(global.size() == 29);

    {
      assert(global.count("positive"));
      const Entry &e = global["positive"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "positive");
    }

    {
      assert(global.count("negative"));
      const Entry &e = global["negative"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "negative");
    }

    {
      assert(global.count("max_heap_size"));
      const Entry &e = global["max_heap_size"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "max_heap_size");
    }

    {
      assert(global.count("character"));
      const Entry &e = global["character"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "character");
    }

    {
      assert(global.count("is_valid"));
      const Entry &e = global["is_valid"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "is_valid");
    }

    {
      assert(global.count("hasattr"));
      const Entry &e = global["hasattr"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "hasattr");
    }

    {
      assert(global.count("digit"));
      const Entry &e = global["digit"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "digit");
    }

    {
      assert(global.count("letter"));
      const Entry &e = global["letter"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "letter");
    }

    {
      assert(global.count("heap"));
      const Entry &e = global["heap"];
      assert(IsInstance<Array>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heap");
    }

    {
      assert(global.count("index"));
      const Entry &e = global["index"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "index");
    }

    {
      assert(global.count("flag"));
      const Entry &e = global["flag"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "flag");
    }

    {
      assert(global.count("array"));
      const Entry &e = global["array"];
      assert(IsInstance<Array>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "array");
    }

    {
      assert(global.count("module"));
      const Entry &e = global["module"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "module");
    }

    {
      assert(global.count("class"));
      const Entry &e = global["class"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "class");
    }

    {
      assert(global.count("errors"));
      const Entry &e = global["errors"];
      assert(IsInstance<Array>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "errors");
    }

    {
      assert(global.count("input"));
      const Entry &e = global["input"];
      assert(IsInstance<Array>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "input");
    }

    {
      assert(global.count("addi"));
      const Entry &e = global["addi"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "addi");
    }

    {
      assert(global.count("addc"));
      const Entry &e = global["addc"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "addc");
    }

    {
      assert(global.count("print_char"));
      const Entry &e = global["print_char"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "print_char");
    }

    {
      assert(global.count("print_string"));
      const Entry &e = global["print_string"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "print_string");
    }

    {
      assert(global.count("print_string_ex"));
      const Entry &e = global["print_string_ex"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "print_string_ex");
    }

    {
      assert(global.count("dead_loop"));
      const Entry &e = global["dead_loop"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "dead_loop");
    }

    {
      assert(global.count("prompt_int"));
      const Entry &e = global["prompt_int"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "prompt_int");
    }

    {
      assert(global.count("swap"));
      const Entry &e = global["swap"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "swap");
    }

    {
      assert(global.count("heapify"));
      const Entry &e = global["heapify"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heapify");
    }

    {
      assert(global.count("heap_sort"));
      const Entry &e = global["heap_sort"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heap_sort");
    }

    {
      assert(global.count("print_heap"));
      const Entry &e = global["print_heap"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "print_heap");
    }

    {
      assert(global.count("reset_heap"));
      const Entry &e = global["reset_heap"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "reset_heap");
    }

    {
      assert(global.count("main"));
      const Entry &e = global["main"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "main");
    }
  }

  assert(locals.find("addi") != locals.end());
  {
    auto &local = locals["addi"];
    assert(local.size() == 2);

    {
      assert(local.count("a"));
      const Entry &e = local["a"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "a");
    }

    {
      assert(local.count("b"));
      const Entry &e = local["b"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "b");
    }
  }

  assert(locals.find("addc") != locals.end());
  {
    auto &local = locals["addc"];
    assert(local.size() == 2);

    {
      assert(local.count("a"));
      const Entry &e = local["a"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "a");
    }

    {
      assert(local.count("b"));
      const Entry &e = local["b"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "b");
    }
  }

  assert(locals.find("print_char") != locals.end());
  {
    auto &local = locals["print_char"];
    assert(local.size() == 1);

    {
      assert(local.count("c"));
      const Entry &e = local["c"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "c");
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
      const Entry &e = local["expr"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "expr");
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
      const Entry &e = local["val"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "val");
    }
  }

  assert(locals.find("swap") != locals.end());
  {
    auto &local = locals["swap"];
    assert(local.size() == 4);

    {
      assert(local.count("index1"));
      const Entry &e = local["index1"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "index1");
    }

    {
      assert(local.count("index2"));
      const Entry &e = local["index2"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "index2");
    }

    {
      assert(local.count("temp"));
      const Entry &e = local["temp"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "temp");
    }

    {
      assert(local.count("heap"));
      const Entry &e = local["heap"];
      assert(IsInstance<Array>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heap");
    }
  }

  assert(locals.find("heapify") != locals.end());
  {
    auto &local = locals["heapify"];
    assert(local.size() == 8);

    {
      assert(local.count("index"));
      const Entry &e = local["index"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "index");
    }

    {
      assert(local.count("heap_size"));
      const Entry &e = local["heap_size"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "heap_size");
    }

    {
      assert(local.count("largest"));
      const Entry &e = local["largest"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "largest");
    }

    {
      assert(local.count("left"));
      const Entry &e = local["left"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "left");
    }

    {
      assert(local.count("right"));
      const Entry &e = local["right"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "right");
    }

    {
      assert(local.count("heap"));
      const Entry &e = local["heap"];
      assert(IsInstance<Array>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heap");
    }

    {
      assert(local.count("swap"));
      const Entry &e = local["swap"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "swap");
    }

    {
      assert(local.count("heapify"));
      const Entry &e = local["heapify"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heapify");
    }
  }

  assert(locals.find("heap_sort") != locals.end());
  {
    auto &local = locals["heap_sort"];
    assert(local.size() == 4);

    {
      assert(local.count("heap_size"));
      const Entry &e = local["heap_size"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "heap_size");
    }

    {
      assert(local.count("i"));
      const Entry &e = local["i"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "i");
    }

    {
      assert(local.count("heapify"));
      const Entry &e = local["heapify"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heapify");
    }

    {
      assert(local.count("swap"));
      const Entry &e = local["swap"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "swap");
    }
  }

  assert(locals.find("print_heap") != locals.end());
  {
    auto &local = locals["print_heap"];
    assert(local.size() == 3);

    {
      assert(local.count("heap_size"));
      const Entry &e = local["heap_size"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "heap_size");
    }

    {
      assert(local.count("i"));
      const Entry &e = local["i"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "i");
    }

    {
      assert(local.count("heap"));
      const Entry &e = local["heap"];
      assert(IsInstance<Array>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heap");
    }
  }

  assert(locals.find("reset_heap") != locals.end());
  {
    auto &local = locals["reset_heap"];
    assert(local.size() == 3);

    {
      assert(local.count("i"));
      const Entry &e = local["i"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "i");
    }

    {
      assert(local.count("max_heap_size"));
      const Entry &e = local["max_heap_size"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "max_heap_size");
    }

    {
      assert(local.count("heap"));
      const Entry &e = local["heap"];
      assert(IsInstance<Array>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heap");
    }
  }

  assert(locals.find("main") != locals.end());
  {
    auto &local = locals["main"];
    assert(local.size() == 10);

    {
      assert(local.count("n"));
      const Entry &e = local["n"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "n");
    }

    {
      assert(local.count("temp"));
      const Entry &e = local["temp"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "temp");
    }

    {
      assert(local.count("i"));
      const Entry &e = local["i"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "i");
    }

    {
      assert(local.count("j"));
      const Entry &e = local["j"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "j");
    }

    {
      assert(local.count("array_size"));
      const Entry &e = local["array_size"];
      assert(IsInstance<Variable>(e.type));
      assert(e.scope == Scope::Local);
      assert(e.name == "array_size");
    }

    {
      assert(local.count("reset_heap"));
      const Entry &e = local["reset_heap"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "reset_heap");
    }

    {
      assert(local.count("max_heap_size"));
      const Entry &e = local["max_heap_size"];
      assert(IsInstance<Constant>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "max_heap_size");
    }

    {
      assert(local.count("heap"));
      const Entry &e = local["heap"];
      assert(IsInstance<Array>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heap");
    }

    {
      assert(local.count("heap_sort"));
      const Entry &e = local["heap_sort"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "heap_sort");
    }

    {
      assert(local.count("print_heap"));
      const Entry &e = local["print_heap"];
      assert(IsInstance<Function>(e.type));
      assert(e.scope == Scope::Global);
      assert(e.name == "print_heap");
    }
  }
}

