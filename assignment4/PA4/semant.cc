#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"

extern int semant_debug;
extern char *curr_filename;

std::map<Class_,std::vector<method_class*> > methodTable; // methods in each class
std::map<Symbol,Class_> classTable; // class and their names
std::vector<Class_> classlist; // list of classes
SymbolTable<Symbol,Symbol> objenv; // name and type of attributes

typedef std::map<Symbol, Class_> ClassTable;
typedef std::vector<method_class*> Methods;

static ostream& error_stream = cerr;
static int semant_errors = 0;
static Class_ curr_class = 0;

static ostream& semant_error() {
    semant_errors++;
    return error_stream;
}

static ostream& semant_error(tree_node *t) {
    error_stream << curr_class->getFileName() << ":" << t->get_line_number() << ": ";
    return semant_error();
}

static ostream& internal_error(int lineno) {
    error_stream << "FATAL:" << lineno << ": ";
    return error_stream;
}

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////

static Symbol
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;

//
// Initializing the predefined symbols.
//

static void initialize_constants(void) {
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}

static void install_basic_classes(void) {
    // The tree package uses these globals to annotate the classes built below.
    // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    //
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object,
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    //
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class =
	class_(IO,
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer.
    //
    Class_ Int_class =
	class_(Int,
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //
    Class_ Str_class =
	class_(Str,
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat,
								      single_Formals(formal(arg, Str)),
								      Str,
								      no_expr()))),
			       single_Features(method(substr,
						      append_Formals(single_Formals(formal(arg, Int)),
								     single_Formals(formal(arg2, Int))),
						      Str,
						      no_expr()))),
	       filename);

    classTable[Object_class->getName()] = Object_class;
    classTable[IO_class->getName()] = IO_class;
    classTable[Int_class->getName()] = Int_class;
    classTable[Bool_class->getName()] = Bool_class;
    classTable[Str_class->getName()] = Str_class;
    classlist.push_back(Object_class);
    classlist.push_back(IO_class);
    classlist.push_back(Int_class);
    classlist.push_back(Bool_class);
    classlist.push_back(Str_class);
}

static void check_class_inheritance(Classes classes)
{
  // make map for classes
  for (int i = classes->first(); classes->more(i); i = classes->next(i))
  {
      curr_class = classes->nth(i);
      Symbol name = curr_class->getName();
      Symbol parent = curr_class->getParentName();
      if (name == SELF_TYPE)
          semant_error(curr_class) << "Redefinition of basic class SELF_TYPE.\n";
      else if (classTable.find(name) != classTable.end())
          semant_error(curr_class) << "Class " << name << " redefined.\n";
      else if (parent == Int or parent == Str or parent == Bool or parent == SELF_TYPE)
          semant_error(curr_class) << "Class " << name << " cannot inherit class " << parent << ".\n";
      else
      {
          classTable[name] = curr_class;
          classlist.push_back(curr_class);
      }
  }
  // inherits from an undefined class
  for (int i = 0 ; i < classlist.size() ; i++)
  {
      curr_class = classlist[i];
      Symbol name = curr_class->getName();
      Symbol parent;
      if (name != Object)
      {
          parent = classTable[curr_class->getParentName()]->getName();
          if(classTable.find(parent) == classTable.end())
              semant_error(curr_class) << "Class " << name << " inherits from an undefined class " << parent << ".\n";
      }
  }
  // involved in an inheritance cycle
  for (int i = 0 ; i < classlist.size() ; i++)
  {
      curr_class = classlist[i];
      Symbol name = curr_class->getName();
      Symbol parent;
      if (name != Object)
      {
          parent = curr_class->getParentName();
          while (parent != Object)
          {
              if (parent == name)
              {
                  semant_error(curr_class) << "Class " << name <<" is involved in an inheritance cycle.\n";
                  break;
              }
              parent = classTable[parent]->getParentName();
          }
      }
  }
}
// get ancestors of a class
static std::vector<Class_> get_parents(Class_ name)
{
    std::vector<Class_> parents;
    while (name->getName() != Object)
    {
        parents.push_back(name);
        if (classTable.find(name->getParentName()) == classTable.end())
            internal_error(__LINE__) << "invalid inheritance parents.\n";
        else
            name = classTable[name->getParentName()];
    }
    parents.push_back(classTable[Object]);
    return parents;
}
// get ancestors of a class
static std::vector<Class_> get_parents(Symbol name) {
    if (name == SELF_TYPE)
        name = curr_class->getName();
    if (classTable.find(name) == classTable.end())
        internal_error(__LINE__) << name << " not found in class table.\n";
    return get_parents(classTable[name]);
}
// check being ancestor of another class
static bool conform(Symbol name1, Symbol name2) {
    if(name1 == name2 or name1 == No_type)
        return true;
    if (name1 == SELF_TYPE && name2 == SELF_TYPE)
        return true;
    if (name1 != SELF_TYPE && name2 == SELF_TYPE)
        return false;
    if (name1 == SELF_TYPE)
        name1 = curr_class->getName();

    if (classTable.find(name1) == classTable.end())
        internal_error(__LINE__) << name1 << " not found in class table.\n";

    if (classTable.find(name2) == classTable.end())
        internal_error(__LINE__) << name2 << " not found in class table.\n";

    Class_ c1 = classTable[name1];
    Class_ c2 = classTable[name2];
    std::vector<Class_> parents = get_parents(c1);

    for (size_t i = 0; i < parents.size(); i++)
        if (parents[i] == c2)
            return true;

    return false;
}

// get the smallest ancestor of a class
static Class_ ldu(Symbol name1, Symbol name2) {
  std::vector<Class_> parents1 = get_parents(name1);
  std::vector<Class_> parents2 = get_parents(name2);
  int size=std::min(parents1.size(), parents2.size());
  for (size_t i = size-2; i > -1 ; i--)
  {
    if (parents1[i] != parents2[i])
        return parents1[i+1];
  }
  return parents1[0];
}
// get a method(same name) from ancestor class
static method_class* getMethod(Class_ clss, Symbol method_name)
{
    Methods methods = methodTable[clss];
    for (size_t i = 0; i < methods.size(); i++)
        if (methods[i]->getName() == method_name)
            return methods[i];
    return 0;
}


static void check_main()
{
  // Class Main is not defined.
  Symbol name = Main;
  if(classTable.find(name) == classTable.end())
  {
      semant_error() << "Class Main is not defined.\n";
  }
  else
  {
      curr_class = classTable[name];
      Features features = curr_class->getFeatures();
      Feature curr_feature;
      bool mainmeth = false;
      bool getargument = false;
      for (int i = features->first(); features->more(i); i = features->next(i))
      {
          curr_feature = features->nth(i);
          if(curr_feature->isMethod())
          {
              method_class* curr_method = static_cast<method_class*> (curr_feature);
              if(curr_method->getName() == main_meth)
              {
                  mainmeth = true;
                  int i = curr_method->getFormals()->first();
                  if(i)
                  {
                      getargument = true;
                  }
              }
          }
      }
      if (not mainmeth)
          semant_error(curr_class) << "No 'main' method in class Main.\n";
      if(getargument)
          semant_error(curr_class) << "'main' method dont get arguments\n";
  }
}

static void check_methods_attrs() {
  // make table for methods
    for (int i = 0 ; i < classlist.size() ; i++)
    {
        curr_class = classlist[i];
        Symbol name = curr_class->getName();
        Features features = curr_class->getFeatures();
        std::vector<method_class*> methods;
        for (int i = features->first(); features->more(i); i = features->next(i))
        {
            if (features->nth(i)->isMethod())
            {
                method_class* method = static_cast<method_class*>(features->nth(i));
                bool existed = false;
                for (int j = 0; j < methods.size(); j++)
                {
                  if (methods[j]->getName() == method->getName())
                      existed = true;
                }
                if (existed)
                    semant_error(curr_class) << "Method " << method->getName() << " is multiply defined.\n";
                else
                    methods.push_back(method);
            }
        }
        methodTable[curr_class] = methods;
    }
    // some error in methods and attributes
    for (int i = 0 ; i < classlist.size() ; i++)
    {
        curr_class = classlist[i];
        Symbol name = curr_class->getName();
        if (name == Object or name == IO or name == Int or name == Bool or name == Str) continue;
        std::vector<Class_> parents = get_parents(curr_class);
        for (int k = parents.size()-1 ; k > 0 ; k--)
        {
            Class_ ancestor_class = parents[k];
            Features ancestor_features = ancestor_class->getFeatures();
            objenv.enterscope();
            for (int i = ancestor_features->first(); ancestor_features->more(i); i = ancestor_features->next(i))
            {
                if (ancestor_features->nth(i)->isAttr())
                {
                    attr_class* attr = static_cast<attr_class*>(ancestor_features->nth(i));
                    objenv.addid(attr->getName(),new Symbol(attr->getType()));
                }
            }
        }
        // check overwrite attributes in parent's class
        Features curr_features = curr_class->getFeatures();
        objenv.enterscope();
        for (int i = curr_features->first(); curr_features->more(i); i = curr_features->next(i))
        {
            if (curr_features->nth(i)->isAttr())
            {
                attr_class* attr = static_cast<attr_class*>(curr_features->nth(i));
                if(objenv.lookup(attr->getName()) != 0)
                    semant_error(curr_class) << "Attribute "<< attr->getName() << " cant be overriden in parents class\n";
                else
                {
                    objenv.addid(attr->getName(),new Symbol(attr->getType()));
                }
            }
        }
        Features features = curr_class->getFeatures();
        for (int i = features->first(); features->more(i); i = features->next(i))
            if (features->nth(i)->isMethod()) {
                method_class* curr_method = static_cast<method_class*>(features->nth(i));
                curr_method->type_checking();
                for (size_t k = 0; k < parents.size(); k++)
                {
                    method_class* ancestor_method = getMethod(parents[k], curr_method->getName());
                    if (!ancestor_method) continue;

                    if (curr_method->getReturnType() != ancestor_method->getReturnType())
                        semant_error(curr_method) << "In redefined method " << curr_method->getName() << ", return type " << curr_method->getReturnType() << " is different from original return type " << ancestor_method->getReturnType() << ".\n";

                    Formals curr_formals = curr_method->getFormals();
                    Formals ancestor_formals = ancestor_method->getFormals();

                    int k1 = curr_formals->first(), k2 = ancestor_formals->first();
                    while (curr_formals->more(k1) && ancestor_formals->more(k2)) {
                        if (curr_formals->nth(k1)->getType() != ancestor_formals->nth(k2)->getType())
                            semant_error(curr_formals->nth(k1)) << "In redefined method " << curr_method->getName() << ", parameter type " << curr_formals->nth(k1)->getType() << " is different from original type " << ancestor_formals->nth(k2)->getType() << ".\n";
                        k1 = curr_formals->next(k1);
                        k2 = ancestor_formals->next(k2);
                        if (curr_formals->more(k1) xor ancestor_formals->more(k2))
                            semant_error(curr_method) << "Incompatible number of formal parameters in redefined method " << curr_method->getName() << ".\n";
                    }
                }
            }
            else
            {
                attr_class* curr_attr = static_cast<attr_class*>(features->nth(i));
                curr_attr->type_checking();
            }

        for (size_t k = 1; k < parents.size(); k++)
            objenv.exitscope();
    }
}




// type checking for all type classes
Symbol int_const_class::type_checking()
{
    type = Int;
    return type;
}
Symbol string_const_class::type_checking()
{
    type = Str;
    return type;
}
Symbol bool_const_class::type_checking()
{
    type = Bool;
    return type;
}
Symbol no_expr_class::type_checking()
{
    type = No_type;
    return type;
}
Symbol assign_class::type_checking()
{
    type = this->expr->type_checking();
    if(objenv.lookup(this->name) == 0)
    {
        semant_error(this) << "Assignment to undeclared variable " << this->name << ".\n";
        type = Object;
        return type;
    }
    if(type != SELF_TYPE and classTable.find(type) == classTable.end())
    {
        semant_error(this) << "undefined class "<<type<<"\n.";
        type = Object;
        return type;
    }
    if(conform(type,*objenv.lookup(this->name)))
        return type;
    type = Object;
    return type;
}
Symbol new__class::type_checking()
{
    type = this->type_name;
    if(this->type_name != SELF_TYPE and classTable.find(this->type_name) == classTable.end())
    {
        semant_error(this) << "undefined class "<<this->type_name<<"\n.";
        type = Object;
        return type;
    }
    return type;
}
Symbol static_dispatch_class::type_checking()
{
    type = this->expr->type_checking();
    if (this->type_name != SELF_TYPE and classTable.find(this->type_name) == classTable.end()) {
        semant_error(this) << "Static dispatch to undefined class " << this->type_name << ".\n";
        type = Object;
        return type;
    }
    if (type != SELF_TYPE and classTable.find(type) == classTable.end())
    {
        semant_error(this) << "Static dispatch on undefined class " << type << ".\n";
        type = Object;
        return type;
    }
    if(not conform(type,this->type_name))
    {
        semant_error(this) << "Expression type " << type << " does not conform to declared static dispatch type " << this->type_name << ".\n";
        type = Object;
        return type;
    }
    method_class* method = 0;
    std::vector<Class_> parents = get_parents(this->type_name);
    for (int i = 0; i < parents.size(); i++)
    {
        for (int j = 0; j < methodTable[parents[i]].size(); j++)
        {
            if (methodTable[parents[i]][j]->getName() == this->name)
            {
                method = methodTable[parents[i]][j];
                break;
            }
        }
        if(method != 0)
            break;
    }
    if (method == 0)
    {
        semant_error(this) << "Dispatch to undefined method " << this->name << ".\n";
        type = Object;
        return type;
    }
    Formals formals = method->getFormals();
    int k1 = this->actual->first(), k2 = formals->first();
    bool error = false;
    while (this->actual->more(k1) && formals->more(k2))
    {
        Symbol actual_type = this->actual->nth(k1)->type_checking();
        Symbol formal_type = formals->nth(k2)->getType();
        if (not conform(actual_type,formal_type))
        {
            error = true;
            semant_error(this) << "In call of method " << this->name << ", type " << actual_type << " of parameter " << formals->nth(k2)->getName() << " does not conform to declared type " << formal_type << ".\n";
        }
        k1 = this->actual->next(k1);
        k2 = formals->next(k2);
        if (this->actual->more(k1) xor formals->more(k2))
        {
            error = true;
            semant_error(this) << "Method " << this->name << " called with wrong number of arguments.\n";
            break;
        }
    }
    if (error)
    {
      type = Object;
      return type;
    }
    if (method->getReturnType() != SELF_TYPE)
        type = method->getReturnType();
    return type;
}
Symbol dispatch_class::type_checking()
{
    type = this->expr->type_checking();
    if (type != SELF_TYPE and classTable.find(type) == classTable.end())
    {
        semant_error(this) << "Dispatch on undefined class " << type << ".\n";
        type = Object;
        return type;
    }
    method_class* method ;
    bool exist = false;
    std::vector<Class_> parents = get_parents(type);
    for (int i = 0; i < parents.size(); i++)
    {
        for (int j = 0; j < methodTable[parents[i]].size(); j++)
        {
            if (methodTable[parents[i]][j]->getName() == this->name)
            {
                method = methodTable[parents[i]][j];
                exist = true;
                break;
            }
        }
        if(exist)
            break;
    }
    if (not exist)
    {
        semant_error(this) << "Dispatch to undefined method " << this->name << ".\n";
        type = Object;
        return type;
    }
    Formals formals = method->getFormals();
    int k1 = this->actual->first(), k2 = formals->first();
    bool error = false;
    while (this->actual->more(k1) && formals->more(k2))
    {
        Symbol actual_type = this->actual->nth(k1)->type_checking();
        Symbol formal_type = formals->nth(k2)->getType();
        if (not conform(actual_type,formal_type))
        {
            error = true;
            semant_error(this) << "In call of method " << this->name << ", type " << actual_type << " of parameter " << formals->nth(k2)->getName() << " does not conform to declared type " << formal_type << ".\n";
        }
        k1 = this->actual->next(k1);
        k2 = formals->next(k2);
        if (this->actual->more(k1) xor formals->more(k2))
        {
            error = true;
            semant_error(this) << "Method " << this->name << " called with wrong number of arguments.\n";
            break;
        }
    }
    if (error)
    {
      type = Object;
      return type;
    }
    if (method->getReturnType() != SELF_TYPE)
        type = method->getReturnType();
    return type;
}
Symbol cond_class::type_checking()
{
    if (this->pred->type_checking() != Bool)
        semant_error(this) << "Predicate of 'if' does not have type Bool.\n";
    Symbol a1 = this->then_exp->type_checking();
    Symbol a2 = this->else_exp->type_checking();
    if (a1 == SELF_TYPE and a2 == SELF_TYPE)
        type = SELF_TYPE;
    else if(classTable.find(a1) == classTable.end() or classTable.find(a2) == classTable.end())
        type = Object;
    else
    {
        type = (ldu(a1, a2)->getName());
    }
    return type;
}
Symbol block_class::type_checking()
{
    bool error = false;
    for (int i = this->body->first(); this->body->more(i); i = this->body->next(i))
    {
        type = this->body->nth(i)->type_checking();
        if(type != SELF_TYPE and classTable.find(type) == classTable.end())
            error = true;
    }
    if(error)
        type = Object;
    return type;
}
Symbol let_class::type_checking()
{
    objenv.enterscope();
    objenv.addid(this->identifier, new Symbol(this->type_decl));
    Symbol init_type = this->init->type_checking();
    if (classTable.find(this->type_decl) == classTable.end())
        semant_error(this) << "Class " << this->type_decl << " of let-bound identifier " << this->identifier << " is undefined.\n";
    else if (!conform(init_type, this->type_decl))
        semant_error(this) << "Inferred type " << init_type << " of initialization of " << this->identifier << " does not conform to identifier's declared type " << this->type_decl << ".\n";
    type = this->body->type_checking();
    if(type != SELF_TYPE and classTable.find(type) == classTable.end())
    {
        semant_error(this) << "Class " << type << " is undefined.\n";
        type = Object;
        return type;
    }
    objenv.exitscope();
    return type;
}
Symbol branch_class::type_checking()
{
    objenv.enterscope();
    objenv.addid(name, new Symbol(this->type_decl));
    type = this->expr->type_checking();
    objenv.exitscope();
    return type;
}
Symbol typcase_class::type_checking()
{
    Symbol expr_type = this->expr->type_checking();
    std::set<Symbol> branch_types;

    for (int i = this->cases->first(); this->cases->more(i); i = this->cases->next(i))
    {
        branch_class* branch = static_cast<branch_class*>(this->cases->nth(i));
        if (branch_types.find(branch->get_type_decl()) != branch_types.end())
            semant_error(this) << "Duplicate branch " << branch->get_type_decl() << " in case statement.\n";
        else
            branch_types.insert(branch->get_type_decl());
        Symbol branch_type = branch->type_checking();
        if (i == this->cases->first())
            type = branch_type;
        else if (type != SELF_TYPE || branch_type != SELF_TYPE)
            type = ldu(type, branch_type)->getName();
    }
    return type;
}
Symbol loop_class::type_checking()
{
    if(this->pred->type_checking() != Bool)
    {
      semant_error(this) << "Loop condition does not have type Bool.\n";
    }
    this->body->type_checking();
    type = Object;
    return type;
}
Symbol isvoid_class::type_checking()
{
    this->e1->type_checking();
    type = Bool;
    return type;
}
Symbol comp_class::type_checking()
{
  type = this->e1->type_checking();
  if (type != Bool)
  {
      semant_error(this) << "Argument of 'not' has type " << type << " instead of Bool.\n";
      type = Object;
      return type;
  }
  type = Bool;
  return type;
}
Symbol neg_class::type_checking()
{
  type = this->e1->type_checking();
  if (type != Int)
  {
      semant_error(this) << "Argument of 'not' has type " << type << " instead of Bool.\n";
      type = Object;
      return type;
  }
  type = Int;
  return type;
}
Symbol plus_class::type_checking()
{
    Symbol type1 = this->e1->type_checking();
    Symbol type2 = this->e2->type_checking();
    if (type1 != Int || type2 != Int)
    {
        semant_error(this) << "non-Int arguments: " << type1 << " + " << type2 << endl;
        type = Object;
        return type;
    }
    type = Int;
    return type;
}

Symbol sub_class::type_checking()
{
    Symbol type1 = this->e1->type_checking();
    Symbol type2 = this->e2->type_checking();
    if (type1 != Int || type2 != Int)
    {
        semant_error(this) << "non-Int arguments: " << type1 << " + " << type2 << endl;
        type = Object;
        return type;
    }
    type = Int;
    return type;
}

Symbol mul_class::type_checking()
{
    Symbol type1 = this->e1->type_checking();
    Symbol type2 = this->e2->type_checking();
    if (type1 != Int || type2 != Int)
    {
        semant_error(this) << "non-Int arguments: " << type1 << " + " << type2 << endl;
        type = Object;
        return type;
    }
    type = Int;
    return type;
}

Symbol divide_class::type_checking()
{
    Symbol type1 = this->e1->type_checking();
    Symbol type2 = this->e2->type_checking();
    if (type1 != Int || type2 != Int)
    {
        semant_error(this) << "non-Int arguments: " << type1 << " + " << type2 << endl;
        type = Object;
        return type;
    }
    type = Int;
    return type;
}
Symbol lt_class::type_checking()
{
    Symbol type1 = this->e1->type_checking();
    Symbol type2 = this->e2->type_checking();
    if (type1 != Int || type2 != Int)
    {
        semant_error(this) << "non-Int arguments: " << type1 << " + " << type2 << endl;
        type = Object;
        return type;
    }
    type = Bool;
    return type;
}

Symbol eq_class::type_checking()
{
    Symbol type1 = this->e1->type_checking();
    Symbol type2 = this->e2->type_checking();
    if (not (type1 == Int || type1 == Bool || type1 == Str ) or not(type2 == Int || type2 == Bool || type2 == Str) or type1 != type2)
    {
        semant_error(this) << "Illegal comparison with a basic type.\n";
        type = Object;
        return type;
    }
    type = Bool;
    return type;
}

Symbol leq_class::type_checking()
{
    Symbol type1 = this->e1->type_checking();
    Symbol type2 = this->e2->type_checking();
    if (type1 != Int || type2 != Int)
    {
        semant_error(this) << "non-Int arguments: " << type1 << " + " << type2 << endl;
        type = Object;
        return type;
    }
    type = Bool;
    return type;
}
Symbol object_class::type_checking()
{
    if (this->name == self)
    {
        type = SELF_TYPE;
    }
    else if (objenv.lookup(this->name) != 0)
    {
        type = *objenv.lookup(this->name);
    }
    else
    {
        semant_error(this) << "Undeclared identifier " << this->name << ".\n";
        type = Object;
    }
    return type;
}
void attr_class::type_checking()
{
    Symbol init_type = this->init->type_checking();
    if (classTable.find(this->type_decl) == classTable.end())
        semant_error(this) << "Class " << this->type_decl << " of attribute " << this->name << " is undefined.\n";
    else if (classTable.find(init_type) != classTable.end() && !conform(init_type, this->type_decl))
        semant_error(this) << "Inferred type " << init_type << " of initialization of attribute " << this->name << " does not conform to declared type " << this->type_decl << ".\n";
}
void method_class::type_checking()
{
    objenv.enterscope();
    for (int i = this->formals->first(); this->formals->more(i); i = this->formals->next(i))
    {
        if (this->formals->nth(i)->getName() == self)
            semant_error(this) << "'self' cannot be the name of a formal parameter.\n";
        else if (objenv.probe(this->formals->nth(i)->getName()) != 0)
            semant_error(this) << "Formal parameter " << this->formals->nth(i)->getName() << " is multiply defined.\n";
        else if (classTable.find(this->formals->nth(i)->getType()) == classTable.end())
            semant_error(this) << "Class " << this->formals->nth(i)->getType() << " of formal parameter " << this->formals->nth(i)->getName() << " is undefined.\n";
        else
            objenv.addid(this->formals->nth(i)->getName(), new Symbol(this->formals->nth(i)->getType()));
    }
    Symbol type = this->expr->type_checking();
    if (this->return_type != SELF_TYPE && classTable.find(this->return_type) == classTable.end())
        semant_error(this) << "Undefined return type " << this->return_type << " in method " << name << ".\n";
    else if (!conform(type, this->return_type))
        semant_error(this) << "Inferred return type " << type << " of method " << this->name << " does not conform to declared return type " << this->return_type << ".\n";
    objenv.exitscope();
}

void program_class::semant()
{
    initialize_constants();
    install_basic_classes();
    check_class_inheritance(classes);
    if (semant_errors > 0) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
    check_main();
    check_methods_attrs();
    if (semant_errors > 0) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
}
