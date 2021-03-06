# Internal Mod Menu Template

## Setting up

```
git clone https://github.com/DaniTRDev/InternalMenuTemplate.git --recursive
```

```
gen_project.bat
```

## Adding a new feature

Git branches are effectively a pointer to a snapshot of your changes. When you want to add a new feature or fix a bug—no matter how big or how small—you spawn a new branch to encapsulate your changes.

```bash
git checkout -b detour_hooking
```

Git checkout is mostly used to swap branches but providing the -b flag it will create a new branch and "check it out".

## Git Commit

When committing try to work with the same methodology as below.

Add files to be committed:
```bash
git add src/some_file.hpp
```

Be descriptive about what you changed:
```bash
git commit -m "feat(SomeFile): Added this feature"
```

When fixing stuff use fix(Something): Something something
```bash
git commit -m "fix(VMTHook): Fixed issue when selecting function index"
```

```bash
git push origin branch_name
```

## Coding Style

Naming convention = PascalCase

```cpp
class ClassName;
inline ClassName* g_ClassName = nullptr;

class ClassName
{
public:
    ClassName(std::string SomeParameter = "Instantiation")
        : m_MemberVariable(SomeParameter)
    {

    }
    virtual ~ClassName()
    {

    }

    const std::string GetMemberVariable()
    {
        return m_MemberVariable;
    }
private:
    std::string m_MemberVariable;

};
```

## File Naming

Use .cpp and .hpp

Use the same name as your class names eg. ClassName => ClassName.hpp / ClassName.cpp

## Thanks to https://github.com/Yimura for this incredible readme and the start up files of the base.
