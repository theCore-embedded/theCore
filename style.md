# theCore C/C++ code style

Below is the style guide for theCore.

**Instead of manually formatting your code, use [the source formatting script](scripts/srcformat.sh).**

## Whitespaces and indentation

 - Do not use tabs.
 Only spaces are allowed.
 - Set indent to 4 spaces per indent.
 - Indent `public`, `private`, `protected` with the same indent as a `class` block.
 - Indent `class` block with the same indent as a namespace:

   ```cpp
   // OK
   namespace foo
   {

   class entity
   {
   public:
       /* */
   private:
       /* */
   protected:   
       /* */
   }

   }
   ```

 - Indent `switch` blocks so that the `case X:`` statements are indented in the switch block:

   ```cpp
    // OK
    switch (foo) {
        case 1:
            a += 1;
            break;

        case 2: {
            a += 2;
            break;
        }
    }
   ```

 - Do not indent labels.
 - Do not indent preprocessor block.
 - Insert space around operators.
 - Insert space padding between `if`, `for`, `while` and the following bracket.
 - **Do not leave trailing whitespaces.**

## Bracket placement

 - Use [One True Bracket Style](https://en.wikipedia.org/wiki/Indent_style#Variant:_1TBS_.28OTBS.29).
 - Place brackets in namespaces, classes, extern-"C" and functions on the next line, e.g:

   ```cpp
   // Wrong
   namespace foo {

   }

   // OK
   namespace bar
   {

   }
   ```

 - Add brackets to unbracketed one line conditional statements (e.g. 'if', 'for', 'while'...).
 - Close whitespace in the angle brackets of template definitions:

   ```cpp
   foo<int, bar<int>> obj;
   ```

## Code length

 - Prefer 80 character code line length.
 100 characters is a maxiumum allowed line length.
 - Prefer to keep functions and methods with not more than 100 lines of code.

## Naming

 - Use [`snake_case`](https://en.wikipedia.org/wiki/Snake_case) for identifer names.
 - Use `m_` prefix when naming private and protected class members.
 - Use `CamelCase` style only for template parameters:

   ```cpp
   template<typename CrazyAllocator>
   class my_strange_container;
   ```  

## Comments

 - **Do not leave commented code.**
 - Prefer not keeping `TODO` in code without correspoding issue.
 Place issue number next to the `TODO` with a little explanation why it is there:

   ```cpp
   // TODO #123: replace foo() with something that exposes smaller IRQ latency.
   foo();
   ```
 - Use doxygen for commenting the function ptototypes and class methods.
 - Use C++ comment lines for doxygen commenting:

   ```cpp
   //! This function does something.
   //! \details Also it calls 911.
   void foo();
   ```

 - Do not use `\brief` doxygen directive.
 Brief tag is set automatically for first comment line.
