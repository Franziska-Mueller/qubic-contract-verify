#include <iostream>

#include <cppparser/cppparser.h>

namespace
{
    bool checkEntity(const cppast::CppEntity& entity, const std::string& stateStructName); 

    bool checkVar(const cppast::CppVar& var, const std::string& stateStructName)
    {
        // TODO: Implement this!
        // 
        // 1.) allowed if it is a global constant and name starts with name of state struct. State struct is the one that inherits from ContractBase
        // 2.) also struct members are allowed! entity.owner returns parent compound (STRUCT vs. BLOCK)

        // not allowed: pointers
        return true;
    }

    bool checkUsingNamespace(const cppast::CppUsingNamespaceDecl& decl)
    {
        // TODO: Implement this!
        // 
        // only allowed if the namespace is QPI
        return true;
    }

    bool checkExpr()
    {
        // TODO: Implement this!
        // 
        // forbidden:
        // - pointers
        // :: (scope resolution, except for structs, enums, and namespaces defined in contracts and qpi.h)
    }

    bool checkCompound(const cppast::CppCompound& compound, const std::string& stateStructName = "")
    {
        return compound.visitAll([&](const cppast::CppEntity& ent) { return checkEntity(ent, stateStructName); });
    }

    bool checkEntity(const cppast::CppEntity& entity, const std::string& stateStructName)
    {
        switch (entity.entityType())
        {
        case cppast::CppEntityType::DOCUMENTATION_COMMENT:
            return true;

        case cppast::CppEntityType::ENTITY_ACCESS_SPECIFIER:
            return true;

        case cppast::CppEntityType::ENUM:
            return true;

        case cppast::CppEntityType::MACRO_CALL:
            return true;

        case cppast::CppEntityType::PREPROCESSOR:
            std::cout << "preprocessor directives are not allowed!" << std::endl;
            return false;

        case cppast::CppEntityType::TYPEDEF_DECL:
            std::cout << "typedef is not allowed!" << std::endl;
            return false;

        case cppast::CppEntityType::TYPEDEF_DECL_LIST:
            std::cout << "typedef is not allowed!" << std::endl;
            return false;

        case cppast::CppEntityType::NAMESPACE_ALIAS:
            std::cout << "namespace alias is not allowed!" << std::endl;
            return false;

        case cppast::CppEntityType::FUNCTION_PTR:
            return false;

        case cppast::CppEntityType::THROW_STATEMENT:
            return false;

        case cppast::CppEntityType::COMPOUND:
            return checkCompound((const cppast::CppCompound&)entity);

        case cppast::CppEntityType::VAR:
            return checkVar((const cppast::CppVar&)entity, stateStructName);

        //case cppast::CppEntityType::VAR_LIST:
        //    return false;

        case cppast::CppEntityType::USING_NAMESPACE:
            return checkUsingNamespace((const cppast::CppUsingNamespaceDecl&)entity);

        
        //case cppast::CppEntityType::TYPE_CONVERTER:

        //case cppast::CppEntityType::EXPRESSION:

        //case cppast::CppEntityType::RETURN_STATEMENT:

        //case cppast::CppEntityType::IF_BLOCK:
        //case cppast::CppEntityType::FOR_BLOCK:
        //case cppast::CppEntityType::WHILE_BLOCK:
        //case cppast::CppEntityType::DO_WHILE_BLOCK:
        //case cppast::CppEntityType::SWITCH_BLOCK:

        //case cppast::CppEntityType::BLOB:

        default:
            return true;

        //case cppast::CppEntityType::CONSTRUCTOR:
        //case cppast::CppEntityType::DESTRUCTOR:
        //case cppast::CppEntityType::FUNCTION:
        //case cppast::CppEntityType::USING_DECL:
        //case cppast::CppEntityType::GOTO_STATEMENT:
        //case cppast::CppEntityType::FORWARD_CLASS_DECL:
        }
    }
}

bool checkCompliance(const cppast::CppCompound& compound, const std::string& stateStructName)
{
    return checkEntity(compound, stateStructName);
}

