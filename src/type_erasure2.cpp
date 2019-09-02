// A Circle implementation of the type erasure tactic implemented here:
// https://github.com/TartanLlama/typeclasses/blob/master/typeclass.hpp

//#include "../gems/util.hxx"
#include <memory>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <type_traits>

/*struct my_interface;
struct forward_t;
struct reverse_t;
struct allcaps_t;*/

// TODO: inherit from multiple typeclass?

// @gen(typeclass)
struct my_interface {
  enum class required {
    print         // Only the print method is required.
  };

  // @gen(required)
  void print(const char* text);

  // @gen(required)
  void print_data();

  void save(const char* filename, const char* access);

  void set_data(const char* text);

  // TODO: pseudo inheritance by code injection
  // @gen(inject)
  void set_interface_data(const char* text) {
    interface_data = text;
  }
  // @gen(inject)
  void print_interface_data() {
    puts(interface_data.c_str());
  }
  // @gen(inject)
  std::string interface_data = "interface_data";
};

// Print the text in forward order.
// @gen(from_typeclass(my_interface))
struct forward_t {
  void print(const char* text) {
    puts(text);
  }

  void save(const char* filename, const char* access) {
    puts("forward_t::save called");
  }

  void set_data(const char* text) {
    forward_t_data = text;
  }

  void print_data() {
    puts(forward_t_data.c_str());
  }

  std::string forward_t_data = "forward_t_data";
};

// Print the text in reverse order.
// @gen(from_typeclass(my_interface))
struct reverse_t {
  void print(const char* text) {
    int len = strlen(text);
    for(int i = 0; i < len; ++i)
      putchar(text[len - 1 - i]);
    putchar('\n');
  }

  void set_data(const char* text) {
    reverse_t_data = text;
  }

  void print_data() {
    puts(reverse_t_data.c_str());
  }

  std::string reverse_t_data = "reverse_t_data";
};

// Print the text with caps.
// @gen(from_typeclass(my_interface))
struct allcaps_t {
  void print(const char* text) {
    while(char c = *text++)
      putchar(toupper(c));
    putchar('\n');
  }

  void set_data(const char* text) {
    allcaps_t_data = text;
  }

  void print_data() {
    puts(allcaps_t_data.c_str());
  }

  std::string allcaps_t_data = "allcaps_t_data";
};

// model_t is the base class for impl_t. impl_t has the storage for the
// object of type_t. model_t has a virtual dtor to trigger impl_t's dtor.
// model_t has a virtual clone function to copy-construct an instance of
// impl_t into heap memory, which is returned via unique_ptr. model_t has
// a pure virtual function for each method in the interface class typeclass.
template<typename typeclass>
struct model_t {
  virtual ~model_t() { }

  virtual std::unique_ptr<model_t> clone() = 0;

  // Loop over each member function on the interface.
  /*@meta for(int i = 0; i < @method_count(typeclass); ++i) {

    @meta std::string func_name = @method_name(typeclass, i);

    // Declare a "has_" function.
    virtual bool @(format("has_%s", func_name.c_str()))() const = 0;

    // Declare a pure virtual function for each interface method.
    virtual @func_decl(@method_type(typeclass, i), func_name, args) = 0;
  }*/
};

template<>
struct model_t<my_interface> {
  virtual ~model_t() { }

  virtual std::unique_ptr<model_t> clone() = 0;
  virtual bool has_save() const = 0;

  //template <typename ...Params>
  //virtual void save(Params&&... args) = 0;
  virtual void save(const char* filename, const char* access) = 0;

  virtual bool has_print() const = 0;

  //template <typename ...Params>
  //virtual void print(Params&&... args) = 0;
  virtual void print(const char* text) = 0;

  virtual void set_data(const char* text) = 0;

  virtual void print_data() = 0;

  // TODO: pseudo inheritance by code injection
  void set_interface_data(const char* text) {
    interface_data = text;
  }
  void print_interface_data() {
    auto out = std::string("interface_data: ") + interface_data;
    puts(out.c_str());
  }
  std::string interface_data = "interface_data";
};

template<typename typeclass, typename type_t>
struct impl_t : public model_t<typeclass> {

  // Construct the embedded concrete type.
  /*template<typename... args_t>
  impl_t(args_t&&... args) : concrete(std::forward<args_t>(args)...) { }

  std::unique_ptr<model_t<typeclass> > clone() override {
    // Copy-construct a new instance of impl_t on the heap.
    return std::make_unique<impl_t>(concrete);
  }*/

  // Loop over each member function on the interface.
  /*@meta for(int i = 0; i < @method_count(typeclass); ++i) {

    @meta std::string func_name = @method_name(typeclass, i);

    @meta bool is_valid = @sfinae(
      std::declval<type_t>().@(func_name)(
        std::declval<@method_params(typeclass, i)>()...
      )
    );

    // Implement the has_XXX function.
    bool @(format("has_%s", func_name.c_str()))() const override {
      return is_valid;
    }

    // Declare an override function with the same signature as the pure virtual
    // function in model_t.
    @func_decl(@method_type(typeclass, i), func_name, args) override {

      @meta if(is_valid || @sfinae(typeclass::required::@(__func__))) {
        // Forward to the correspondingly-named member function in type_t.
        return concrete.@(__func__)(std::forward<decltype(args)>(args)...);

      } else {

        // We could also call __cxa_pure_virtual or std::terminate here.
        throw std::runtime_error(@string(format("%s::%s not implemented",
          @type_name(type_t), __func__
        )));
      }
    }
  }*/

  // Our actual data.
  //type_t concrete;
};

template<>
struct impl_t<my_interface, allcaps_t> : public model_t<my_interface> {

  // Construct the embedded concrete type.
  template<typename... args_t>
  impl_t(args_t&&... args) : concrete(std::forward<args_t>(args)...) { }

  explicit impl_t(const allcaps_t& concrete_arg)
    : concrete(concrete_arg) {}

  std::unique_ptr<model_t<my_interface> > clone() override {
    // Copy-construct a new instance of impl_t on the heap.
    return std::make_unique<impl_t>(concrete);
  }

  bool has_save() const override {
    return false;
  }

  void save(const char* filename, const char* access) override {
    // TODO: noexcept
    throw std::runtime_error("allcaps_t::save not implemented");
  }

  /*template <typename ...Params>
  void print(Params&&... args) override {
    return concrete.print(std::forward<decltype(args)>(args)...);
  }*/

  bool has_print() const override {
    return true;
  }

  void print(const char* text) override {
    return concrete.print(std::forward<decltype(text)>(text));
  }

  void set_data(const char* text) override {
    return concrete.set_data(std::forward<decltype(text)>(text));
  }

  void print_data() override {
    return concrete.print_data();
  }

  /*void set_interface_data(const char* text) {
    return set_interface_data(std::forward<decltype(text)>(text));
  }

  void print_interface_data() {
    return print_interface_data();
  }*/

  /*template <typename ...Params>
  void save(Params&&... args) override {
    throw std::runtime_error("allcaps_t::save not implemented");
  }*/

  // Loop over each member function on the interface.
  /*@meta for(int i = 0; i < @method_count(typeclass); ++i) {

    @meta std::string func_name = @method_name(typeclass, i);

    @meta bool is_valid = @sfinae(
      std::declval<type_t>().@(func_name)(
        std::declval<@method_params(typeclass, i)>()...
      )
    );

    // Implement the has_XXX function.
    bool @(format("has_%s", func_name.c_str()))() const override {
      return is_valid;
    }

    // Declare an override function with the same signature as the pure virtual
    // function in model_t.
    @func_decl(@method_type(typeclass, i), func_name, args) override {

      @meta if(is_valid || @sfinae(typeclass::required::@(__func__))) {
        // Forward to the correspondingly-named member function in type_t.
        return concrete.@(__func__)(std::forward<decltype(args)>(args)...);

      } else {

        // We could also call __cxa_pure_virtual or std::terminate here.
        throw std::runtime_error(@string(format("%s::%s not implemented",
          @type_name(type_t), __func__
        )));
      }
    }
  }*/

  // Our actual data.
  allcaps_t concrete;
};

template<>
struct impl_t<my_interface, forward_t> : public model_t<my_interface> {

  // Construct the embedded concrete type.
  template<typename... args_t>
  impl_t(args_t&&... args)
    : concrete(std::forward<args_t>(args)...) { }

  explicit impl_t(const forward_t& concrete_arg)
    : concrete(concrete_arg) {}

  std::unique_ptr<model_t<my_interface> > clone() override {
    // Copy-construct a new instance of impl_t on the heap.
    return std::make_unique<impl_t>(concrete);
  }

  bool has_save() const override {
    return true;
  }

  void save(const char* filename, const char* access) override {
    return concrete.save(std::forward<decltype(filename)>(filename),
      std::forward<decltype(access)>(access));
  }

  /*template <typename ...Params>
  void save(Params&&... args) override {
    return concrete.save(std::forward<decltype(args)>(args)...);
  }*/

  bool has_print() const override {
    return true;
  }

  void print(const char* text) override {
    return concrete.print(std::forward<decltype(text)>(text));
  }

  void set_data(const char* text) override {
    return concrete.set_data(std::forward<decltype(text)>(text));
  }

  void print_data() override {
    return concrete.print_data();
  }

  /*void set_interface_data(const char* text) {
    return set_interface_data(std::forward<decltype(text)>(text));
  }

  void print_interface_data() {
    return print_interface_data();
  }*/

  /*template <typename ...Params>
  void print(Params&&... args) override {
    return concrete.print(std::forward<decltype(args)>(args)...);
  }*/

  // Loop over each member function on the interface.
  /*@meta for(int i = 0; i < @method_count(typeclass); ++i) {

    @meta std::string func_name = @method_name(typeclass, i);

    @meta bool is_valid = @sfinae(
      std::declval<type_t>().@(func_name)(
        std::declval<@method_params(typeclass, i)>()...
      )
    );

    // Implement the has_XXX function.
    bool @(format("has_%s", func_name.c_str()))() const override {
      return is_valid;
    }

    // Declare an override function with the same signature as the pure virtual
    // function in model_t.
    @func_decl(@method_type(typeclass, i), func_name, args) override {

      @meta if(is_valid || @sfinae(typeclass::required::@(__func__))) {
        // Forward to the correspondingly-named member function in type_t.
        return concrete.@(__func__)(std::forward<decltype(args)>(args)...);

      } else {

        // We could also call __cxa_pure_virtual or std::terminate here.
        throw std::runtime_error(@string(format("%s::%s not implemented",
          @type_name(type_t), __func__
        )));
      }
    }
  }*/

  // Our actual data.
  forward_t concrete;
};


template<>
struct impl_t<my_interface, reverse_t> : public model_t<my_interface> {

  // Construct the embedded concrete type.
  template<typename... args_t>
  impl_t(args_t&&... args)
    : concrete(std::forward<args_t>(args)...) { }

  explicit impl_t(const reverse_t& concrete_arg)
    : concrete(concrete_arg) {}

  std::unique_ptr<model_t<my_interface> > clone() override {
    // Copy-construct a new instance of impl_t on the heap.
    return std::make_unique<impl_t>(concrete);
  }

  bool has_save() const override {
    return false;
  }

  void save(const char* filename, const char* access) override {
    // TODO: noexcept
    throw std::runtime_error("reverse_t::save not implemented");
  }

  /*template <typename ...Params>
  void save(Params&&... args) override {
    return concrete.save(std::forward<decltype(args)>(args)...);
  }*/

  bool has_print() const override {
    return true;
  }

  void print(const char* text) override {
    return concrete.print(std::forward<decltype(text)>(text));
  }

  void set_data(const char* text) override {
    return concrete.set_data(std::forward<decltype(text)>(text));
  }

  void print_data() override {
    return concrete.print_data();
  }

  /*void set_interface_data(const char* text) {
    return set_interface_data(std::forward<decltype(text)>(text));
  }

  void print_interface_data() {
    return print_interface_data();
  }*/

  /*template <typename ...Params>
  void print(Params&&... args) override {
    return concrete.print(std::forward<decltype(args)>(args)...);
  }*/

  // Loop over each member function on the interface.
  /*@meta for(int i = 0; i < @method_count(typeclass); ++i) {

    @meta std::string func_name = @method_name(typeclass, i);

    @meta bool is_valid = @sfinae(
      std::declval<type_t>().@(func_name)(
        std::declval<@method_params(typeclass, i)>()...
      )
    );

    // Implement the has_XXX function.
    bool @(format("has_%s", func_name.c_str()))() const override {
      return is_valid;
    }

    // Declare an override function with the same signature as the pure virtual
    // function in model_t.
    @func_decl(@method_type(typeclass, i), func_name, args) override {

      @meta if(is_valid || @sfinae(typeclass::required::@(__func__))) {
        // Forward to the correspondingly-named member function in type_t.
        return concrete.@(__func__)(std::forward<decltype(args)>(args)...);

      } else {

        // We could also call __cxa_pure_virtual or std::terminate here.
        throw std::runtime_error(@string(format("%s::%s not implemented",
          @type_name(type_t), __func__
        )));
      }
    }
  }*/

  // Our actual data.
  reverse_t concrete;
};

////////////////////////////////////////////////////////////////////////////////
// var_t is an 8-byte type that serves as the common wrapper for the
// type-erasure model_t. It implements move

template<typename typeclass>
struct var_t {
  // Default initializer creates an empty var_t.
  //var_t() = default;

  /*// Allow initialization from a unique_ptr.
  var_t(std::unique_ptr<model_t<typeclass> >&& model) :
    model(std::move(model)) { }

  // Move ctor/assign by default.
  var_t(var_t&&) = default;
  var_t& operator=(var_t&&) = default;

  // Call clone for copy ctor/assign.
  var_t(const var_t& rhs) {
    if(rhs)
      model = rhs.model->clone();
  }

  var_t& operator=(const var_t& rhs) {
    model.reset();
    if(rhs)
      model = rhs.model->clone();
    return *this;
  }*/

  // A virtual dtor triggers the dtor in the impl.
  //virtual ~var_t() { }

  /*// The preferred initializer for a var_t. This constructs an impl_t of
  // type_t on the heap, and stores the pointer in a new var_t.
  template<typename type_t, typename... args_t>
  static var_t construct(args_t&&... args) {
    return var_t(std::make_unique<impl_t<typeclass, type_t> >(
      std::forward<args_t>(args)...
    ));
  }

  // Loop over each member function on the interface.
  @meta for(int i = 0; i < @method_count(typeclass); ++i) {

    // Define a has_XXX member function.
    bool @(format("has_%s", @method_name(typeclass, i)))() const {
      @meta if(@sfinae(typeclass::required::@(@method_name(typeclass, i))))
        return true;
      else
        return model->@(__func__)();
    }

    // Declare a non-virtual forwarding function for each interface method.
    @func_decl(@method_type(typeclass, i), @method_name(typeclass, i), args) {
      // Forward to the model's virtual function.
      return model->@(__func__)(std::forward<decltype(args)>(args)...);
    }
  }

  explicit operator bool() const {
    return (bool)model;
  }

  // This is actually a unique_ptr to an impl type. We store a pointer to
  // the base type and rely on model_t's virtual dtor to free the object.
  std::unique_ptr<model_t<typeclass> > model;*/
};

template<>
struct var_t<my_interface> {
  // Default initializer creates an empty var_t.
  var_t() = default;

  template <class T>
  var_t(std::unique_ptr<T>&& u) :
    model(std::move(u)) {
    puts("var_t{unique_ptr} called");
  }

  template <class T>
  var_t(const T&& u) :
      model(
        std::make_unique<
          impl_t<my_interface, T>>
        (std::forward<const std::decay_t<T>>(u))) {
    puts("var_t{T} called");
  }

  // Move ctor/assign by default.
  var_t(var_t&&) = default;
  var_t& operator=(var_t&&) = default;

  // Call clone for copy ctor/assign.
  var_t(const var_t& rhs) {
    if(rhs)
      model = rhs.model->clone();
  }

  var_t& operator=(const var_t& rhs) {
    model.reset();
    if(rhs)
      model = rhs.model->clone();
    return *this;
  }

  // A virtual dtor triggers the dtor in the impl.
  virtual ~var_t() { }

  // The preferred initializer for a var_t. This constructs an impl_t of
  // type_t on the heap, and stores the pointer in a new var_t.
  template<typename type_t, typename... args_t>
  static var_t construct(args_t&&... args) {
    return var_t(std::make_unique<impl_t<my_interface, type_t> >(
      std::forward<args_t>(args)...
    ));
  }

  bool has_save() const {
    return model->has_save(); // force to true if required
  }

  template <typename ...Params>
  void save(Params&&... args) {
    return model->save(std::forward<decltype(args)>(args)...);
  }

  bool has_print() const {
    return model->has_print(); // force to true if required
  }

  template <typename ...Params>
  void print(Params&&... args) {
    return model->print(std::forward<decltype(args)>(args)...);
  }

  template <typename ...Params>
  void set_data(Params&&... args) {
    return model->set_data(std::forward<decltype(args)>(args)...);
  }

  template <typename ...Params>
  void print_data(Params&&... args) {
    return model->print_data(std::forward<decltype(args)>(args)...);
  }

  template <typename ...Params>
  void set_interface_data(Params&&... args) {
    return model->set_interface_data(std::forward<decltype(args)>(args)...);
  }

  template <typename ...Params>
  void print_interface_data(Params&&... args) {
    return model->print_interface_data(std::forward<decltype(args)>(args)...);
  }

  /*void print(const char* text) {
    return model->print(std::forward<decltype(text)>(text));
  }*/

  /*// Loop over each member function on the interface.
  @meta for(int i = 0; i < @method_count(my_interface); ++i) {

    // Define a has_XXX member function.
    bool @(format("has_%s", @method_name(my_interface, i)))() const {
      @meta if(@sfinae(my_interface::required::@(@method_name(my_interface, i))))
        return true;
      else
        return model->@(__func__)();
    }

    // Declare a non-virtual forwarding function for each interface method.
    @func_decl(@method_type(my_interface, i), @method_name(my_interface, i), args) {
      // Forward to the model's virtual function.
      return model->@(__func__)(std::forward<decltype(args)>(args)...);
    }
  }*/

  explicit operator bool() const {
    return (bool)model;
  }

  /*bool is_valid() const {
    return (bool)model;
  }*/

  // This is actually a unique_ptr to an impl type. We store a pointer to
  // the base type and rely on model_t's virtual dtor to free the object.
  std::unique_ptr<model_t<my_interface> > model;
};

////////////////////////////////////////////////////////////////////////////////
// The var_t class template is specialized to include all member functions in
// my_interface. It makes forwarding calls from these to the virtual
// functions in model_t.

// The typedef helps emphasize that we have a single type that encompasses
// multiple impl types that aren't related by inheritance.
typedef var_t<my_interface> obj_t;

#if 0
int main() {

  // Construct an object a.
  obj_t a = obj_t::construct<allcaps_t>();
  a.print("Hello a");

  // Copy-construc a to get b.
  obj_t b = a;
  b.print("Hello b");

  if(b.has_save())
    b.save("my.save", "w");

  // Copy-assign a to get c.
  obj_t c;
  c = b;
  c.print("Hello c");

  // Create a forward object.
  obj_t d = obj_t::construct<forward_t>();
  d.print("Hello d");
  d.save("foo.save", "w");

  // Create a reverse object.
  obj_t e = obj_t::construct<reverse_t>();
  e.print("Hello e");

  // Throws:
  // terminate called after throwing an instance of 'std::runtime_error'
  //   what():  reverse_t::save not implemented
  e.save("bar.save", "w");

  return 0;
}
#endif // 0
