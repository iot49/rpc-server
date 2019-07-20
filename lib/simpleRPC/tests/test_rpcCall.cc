#include <catch.hpp>

#include "../src/rpcCall.tcc"


TEST_CASE("RPC call function", "[call][basic]") {
  struct S {
    static void f0(void) {}
    static void f1(int) {}
    static void f2(int, char) {}
    static void f3(int, char, float) {}
    static short int f4(void) {
      return 1;
    }
    static short int f5(int) {
      return 2;
    }
    static short int f6(int, char) {
      return 3;
    }
    static float f7(void) {
      return 1.0F;
    }
    static float f8(int) {
      return 2.0F;
    }
    static float f9(int, char) {
      return 3.0F;
    }
  };

  // Void function, no parameters.
  Serial.reset();
  rpcCall(S::f0);
  REQUIRE(Serial.rx == 0);
  REQUIRE(Serial.tx == 0);

  // Void function, one parameter.
  Serial.reset();
  rpcCall(S::f1);
  REQUIRE(Serial.rx == sizeof(int));
  REQUIRE(Serial.tx == 0);

  // Void function, two parameters.
  Serial.reset();
  rpcCall(S::f2);
  REQUIRE(Serial.rx == sizeof(int) + sizeof(char));
  REQUIRE(Serial.tx == 0);

  // Void function, three parameters.
  Serial.reset();
  rpcCall(S::f3);
  REQUIRE(Serial.rx == sizeof(int) + sizeof(char) + sizeof(float));
  REQUIRE(Serial.tx == 0);

  // Function of type short int, zero parameters.
  Serial.reset();
  rpcCall(S::f4);
  REQUIRE(Serial.inspect<short int>() == 1);
  REQUIRE(Serial.rx == 0);
  REQUIRE(Serial.tx == sizeof(short int));

  // Function of type short int, one parameter.
  Serial.reset();
  rpcCall(S::f5);
  REQUIRE(Serial.inspect<short int>() == 2);
  REQUIRE(Serial.rx == sizeof(int));
  REQUIRE(Serial.tx == sizeof(short int));

  // Function of type short int, two parameters.
  Serial.reset();
  rpcCall(S::f6);
  REQUIRE(Serial.inspect<short int>() == 3);
  REQUIRE(Serial.rx == sizeof(int) + sizeof(char));
  REQUIRE(Serial.tx == sizeof(short int));

  // Function of type float, zero parameters.
  Serial.reset();
  rpcCall(S::f7);
  REQUIRE(Serial.inspect<float>() == 1.0F);
  REQUIRE(Serial.rx == 0);
  REQUIRE(Serial.tx == sizeof(float));

  // Function of type float, one parameter.
  Serial.reset();
  rpcCall(S::f8);
  REQUIRE(Serial.inspect<float>() == 2.0F);
  REQUIRE(Serial.rx == sizeof(int));
  REQUIRE(Serial.tx == sizeof(float));

  // Function of type float, two parameters.
  Serial.reset();
  rpcCall(S::f9);
  REQUIRE(Serial.inspect<float>() == 3.0F);
  REQUIRE(Serial.rx == sizeof(int) + sizeof(char));
  REQUIRE(Serial.tx == sizeof(float));
}

TEST_CASE("RPC call function with String types", "[call][string]") {
  struct S {
    static void f0(String, int) {}
    static void f1(int, String) {}
    static String f2(void) {
      return "xyz";
    }
  };

  // Void function, first parameter is of type String.
  Serial.reset();
  Serial.prepare("xyz", 1234);
  rpcCall(S::f0);
  REQUIRE(Serial.rx == 4 + sizeof(int));
  REQUIRE(Serial.tx == 0);

  // Void function, second parameter is of type String.
  Serial.reset();
  Serial.prepare(1234, "xxx");
  rpcCall(S::f1);
  REQUIRE(Serial.rx == sizeof(int) + 4);
  REQUIRE(Serial.tx == 0);

  // Function with return type String. 
  Serial.reset();
  rpcCall(S::f2);
  REQUIRE(Serial.inspect<String>() == "xyz");
  REQUIRE(Serial.rx == 0);
  REQUIRE(Serial.tx == 4);
}

TEST_CASE("RPC call function with Tuple types", "[call][tuple]") {
  struct S {
    static void f0(Tuple <int, char>) {}
    static Tuple <int, char>f1(void) {
      Tuple <int, char>t = {1234, 'x'};

      return t;
    }
    static void f2(Tuple <int, Vector <char>> &) {}
  };

  // Void function, parameter is of type Tuple.
  Serial.reset();
  Serial.prepare(1234, 'x');
  rpcCall(S::f0);
  REQUIRE(Serial.rx == sizeof(int) + sizeof(char));
  REQUIRE(Serial.tx == 0);

  // Function with return type Tuple.
  Serial.reset();
  rpcCall(S::f1);
  REQUIRE(Serial.inspect<int>() == 1234);
  REQUIRE(Serial.inspect<char>() == 'x');
  REQUIRE(Serial.rx == 0);
  REQUIRE(Serial.tx == sizeof(int) + sizeof(char));

  Serial.reset();
  Serial.prepare(1234, (size_t)2, 'x', 'y');
  rpcCall(S::f2);
  REQUIRE(Serial.rx == sizeof(int) + sizeof(size_t) + 2 * sizeof(char));
  REQUIRE(Serial.tx == 0);
}

TEST_CASE("RPC call function with Object types", "[call][object]") {
  struct S {
    static void f0(Object <int, char> &) {}
    static Object <int, char>f1(void) {
      Object <int, char>o = {1234, 'x'};

      return o;
    }
  };

  // Void function, parameter is of type Object.
  Serial.reset();
  Serial.prepare(1234, 'x');
  rpcCall(S::f0);
  REQUIRE(Serial.rx == sizeof(int) + sizeof(char));
  REQUIRE(Serial.tx == 0);

  // Function with return type Object.
  Serial.reset();
  rpcCall(S::f1);
  REQUIRE(Serial.inspect<int>() == 1234);
  REQUIRE(Serial.inspect<char>() == 'x');
  REQUIRE(Serial.rx == 0);
  REQUIRE(Serial.tx == sizeof(int) + sizeof(char));
}

TEST_CASE("RPC call function with Vector types", "[call][vector]") {
  struct S {
    static bool f0(Vector <int> &v) {
      if (v.size != 2 || v[0] != 1234 || v[1] != 2345) {
        return false;
      }
      return true;
    }
    static Vector <int>f1(void) {
      Vector <int>v(2);

      v[0] = 1234;
      v[1] = 2345;

      return v;
    }
    static void f2(Object <Vector <int>, char> &) {}
    static int f3(Vector<signed char> &, int i)
    {
        return 1;
    }
  };

  // Void function, parameter is of type Vector.
  Serial.reset();
  Serial.prepare((size_t)2, 1234, 2345);
  rpcCall(S::f0);
  REQUIRE(Serial.inspect<bool>());
  REQUIRE(Serial.rx == sizeof(size_t) + 2 * sizeof(int));
  REQUIRE(Serial.tx == 1);

  // Function with return type Vector.
  Serial.reset();
  rpcCall(S::f1);
  REQUIRE(Serial.inspect<size_t>() == 2);
  REQUIRE(Serial.inspect<int>() == 1234);
  REQUIRE(Serial.inspect<int>() == 2345);
  REQUIRE(Serial.rx == 0);
  REQUIRE(Serial.tx == sizeof(size_t) + 2 * sizeof(int));

  Serial.reset();
  Serial.prepare((size_t)2, 1234, 2345, 'c');
  rpcCall(S::f2);
  REQUIRE(Serial.rx == sizeof(size_t) + 2 * sizeof(int) + sizeof(char));
  REQUIRE(Serial.tx == 0);

  Serial.reset();
  Serial.prepare((size_t)2, (signed char)123, (signed char)234, 10);
  rpcCall(S::f3);
  REQUIRE(Serial.rx == sizeof(size_t) + 2 * sizeof(signed char) + sizeof(int));
  REQUIRE(Serial.tx == sizeof(int));
}

TEST_CASE("RPC call class member functions", "[call][class]") {
  class C {
    public:
      void f0(int, char) {}
      short int f1(int, char) {
        return 1;
      }
  };

  C c;

  // Void function.
  Serial.reset();
  rpcCall(pack(&c, &C::f0));
  REQUIRE(Serial.rx == sizeof(int) + sizeof(char));
  REQUIRE(Serial.tx == 0);

  // Non-void function.
  Serial.reset();
  rpcCall(pack(&c, &C::f1));
  REQUIRE(Serial.inspect<short int>() == 1);
  REQUIRE(Serial.rx == sizeof(int) + sizeof(char));
  REQUIRE(Serial.tx == sizeof(short int));
}

TEST_CASE("Executing a function", "[call]") {
  struct S {
    static short int f(int i, char c) {
      return i + c + 1;
    }
  };

  Serial.reset();
  Serial.prepare(1234, '\3');
  rpcCall(S::f);
  REQUIRE(Serial.rx == sizeof(int) + sizeof(char));
  REQUIRE(Serial.tx == sizeof(short int));
  REQUIRE(Serial.inspect<short int>() == 1238);
}
