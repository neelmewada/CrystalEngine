
#include "HeaderAST.h"

#include <stack>

namespace CE
{
	HeaderAST::HeaderAST()
	{

	}

	HeaderAST::~HeaderAST()
	{
		
	}

    HeaderAST* HeaderAST::ProcessHeader(HeaderTokenizer* tokens)
    {
        auto ast = new HeaderAST;
        
        ClassInfo* curClass = nullptr;
        StructInfo* curStruct = nullptr;
        EnumInfo* curEnum = nullptr;
        int curClassScope = 0;
        int curStructScope = 0;
        
        int curScope = 0;
        int curParenthesis = 0;
        bool isCEClassBody = false;
        auto size = tokens->tokens.GetSize();
        
        // namespace
        struct NamespaceInfo
        {
            Name name{};
            int curScope = 0;
        };
        
        std::stack<NamespaceInfo> curNamespace{};
        bool namespaceNameInProgress = false;
        
        for (int i = 0; i < size; i++)
        {
            const auto& token = tokens->tokens[i];
            
            if (token.type == TK_KW_NAMESPACE)
            {
                namespaceNameInProgress = true;
            }
            else if (token.type == TK_SCOPE_OPEN)
            {
                curScope++;
                if (curClass != nullptr)
                {
                    curClassScope++;
                }
                if (curStruct != nullptr)
                {
                    curStructScope++;
                }
                if (namespaceNameInProgress)
                {
                    namespaceNameInProgress = false;
                }

                if (curClass != nullptr && i > 0 && !curClass->name.IsValid())
                {
                    String nameSpace = "";
                    std::stack<NamespaceInfo> copy = curNamespace;
                    while (copy.size() > 0)
                    {
                        if (nameSpace.IsEmpty())
                            nameSpace = copy.top().name.GetString();
                        else
                            nameSpace = copy.top().name.GetString() + "::" + nameSpace;
                        copy.pop();
                    }
                    curClass->nameSpace = nameSpace;
                    int backIdx = i - 1;
                    while (backIdx > 0 && !curClass->name.IsValid())
                    {
                        if (tokens->tokens[backIdx].type == TK_IDENTIFIER)
                            curClass->name = String(tokens->tokens[backIdx].lexeme);
                        backIdx--;
                    }
                }
                else if (curStruct != nullptr && i > 0 && !curStruct->name.IsValid())
                {
                    String nameSpace = "";
                    std::stack<NamespaceInfo> copy = curNamespace;
                    while (copy.size() > 0)
                    {
                        if (nameSpace.IsEmpty())
                            nameSpace = copy.top().name.GetString();
                        else
                            nameSpace = copy.top().name.GetString() + "::" + nameSpace;
                        copy.pop();
                    }
                    curStruct->nameSpace = nameSpace;
                    int backIdx = i - 1;
                    while (backIdx > 0 && !curStruct->name.IsValid())
                    {
                        if (tokens->tokens[backIdx].type == TK_IDENTIFIER)
                            curStruct->name = String(tokens->tokens[backIdx].lexeme);
                        backIdx--;
                    }
                }
            }
            else if (token.type == TK_SCOPE_CLOSE)
            {
                curScope--;
                if (curNamespace.size() > 0 && curNamespace.top().curScope == curScope)
                {
                    curNamespace.pop();
                }
                if (curClass != nullptr)
                {
                    curClassScope--;
                    if (curClassScope <= 0)
                    {
                        curClassScope = 0;
                        ast->classes.Add(*curClass);
                        delete curClass;
                        curClass = nullptr;
                    }
                }
                if (curStruct != nullptr)
                {
                    curStructScope--;
                    if (curStructScope <= 0)
                    {
                        curStructScope = 0;
                        ast->structs.Add(*curStruct);
                        delete curStruct;
                        curStruct = nullptr;
                    }
                }
            }
            else if (token.type == TK_SCOPE_OPERATOR)
            {
                
            }
            else if (token.type == TK_CE_CLASS_BODY && curClass != nullptr)
            {
                int curEntry = 0;
                String baseClassName = "";
                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_COMMA || tokens->tokens[i].type == TK_PAREN_CLOSE)
                    {
                        if (curEntry > 0 && !baseClassName.IsEmpty())
                        {
                            curClass->superClasses.Add(baseClassName);
                            baseClassName = "";
                        }
                        curEntry++;
                    }
                    else if (curEntry > 0 && tokens->tokens[i].type == TK_IDENTIFIER)
                    {
                        if (!baseClassName.IsEmpty())
                            baseClassName += "::";
                        baseClassName += tokens->tokens[i].lexeme;
                    }
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                        break;
                    i++;
                }
            }
            else if (token.type == TK_CE_STRUCT_BODY && curStruct != nullptr)
            {
                int curEntry = 0;
                String baseStructName = "";
                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_COMMA || tokens->tokens[i].type == TK_PAREN_CLOSE)
                    {
                        if (curEntry > 0 && !baseStructName.IsEmpty())
                        {
                            curStruct->superClasses.Add(baseStructName);
                            baseStructName = "";
                        }
                        curEntry++;
                    }
                    else if (curEntry > 0 && tokens->tokens[i].type == TK_IDENTIFIER)
                    {
                        if (!baseStructName.IsEmpty())
                            baseStructName += "::";
                        baseStructName += tokens->tokens[i].lexeme;
                    }
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                        break;
                    i++;
                }
            }
            else if (token.type == TK_PAREN_OPEN)
            {
                curParenthesis++;
            }
            else if (token.type == TK_PAREN_CLOSE)
            {
                curParenthesis--;
            }
            else if (token.type == TK_CE_STRUCT)
            {
                delete curStruct;
                curStruct = new StructInfo;
                int attribScope = 0;
                String curAttrib = "";
                i++;

                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE && attribScope <= 1)
                        break;
                    if (tokens->tokens[i].type == TK_PAREN_OPEN)
                    {
                        attribScope++;
                        if (attribScope > 1)
                            curAttrib += "(";
                    }
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                    {
                        if (attribScope > 1)
                            curAttrib += ")";
                        attribScope--;
                    }
                    if (attribScope == 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        if (!curAttrib.IsEmpty())
                            curStruct->attribs.Add(curAttrib);
                        curAttrib = "";
                    }
                    else if (attribScope > 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        curAttrib += ",";
                    }
                    else if (attribScope >= 1)
                    {
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        curAttrib += String(tokens->tokens[i].lexeme);
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                    }
                    i++;
                }

                if (!curAttrib.IsEmpty())
                {
                    curStruct->attribs.Add(curAttrib);
                    curAttrib = "";
                }
            }
            else if (token.type == TK_CE_CLASS)
            {
                delete curClass;
                curClass = new ClassInfo;
                int attribScope = 0;
                String curAttrib = "";
                i++;
                
                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE && attribScope <= 1)
                        break;
                    if (tokens->tokens[i].type == TK_PAREN_OPEN)
                    {
                        attribScope++;
                        if (attribScope > 1)
                            curAttrib += "(";
                    }
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                    {
                        if (attribScope > 1)
                            curAttrib += ")";
                        attribScope--;
                    }
                    if (attribScope == 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        if (!curAttrib.IsEmpty())
                            curClass->attribs.Add(curAttrib);
                        curAttrib = "";
                    }
                    else if (attribScope > 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        curAttrib += ",";
                    }
                    else if (attribScope >= 1)
                    {
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        curAttrib += String(tokens->tokens[i].lexeme);
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                    }
                    i++;
                }
                
                if (!curAttrib.IsEmpty())
                {
                    curClass->attribs.Add(curAttrib);
                    curAttrib = "";
                }
            }
            else if (token.type == TK_CE_SIGNAL && curClass != nullptr)
            {
                FunctionInfo curFunction{};
                int signalScope = 0;
                i++;

                curFunction.isSignal = true;
                curFunction.attribs.Add("Signal");
                curFunction.isEvent = false;

                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_PAREN_OPEN)
                    {
                        signalScope++;
                    }
                    else if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                    {
                        signalScope--;
                        if (signalScope == 0)
                        {
                            curFunction.signature += ")";
                            i++;
                            if (curFunction.name.IsValid())
                            {
                                curClass->functions.Add(curFunction);
                            }
                            break;
                        }
                    }
                    else if (tokens->tokens[i].type == TK_IDENTIFIER)
                    {
                        if (!curFunction.name.IsValid())
                        {
                            curFunction.name = String(tokens->tokens[i].lexeme);
                            curFunction.signature += "(";
                            i++;
                        }
                        else
                        {
                            curFunction.signature += String(tokens->tokens[i].lexeme);
                        }
                    }
                    else if (curFunction.name.IsValid() && signalScope > 0)
                    {
                        if (tokens->tokens[i].type == TK_COMMA)
                            curFunction.signature += ",";
                        else if (tokens->tokens[i].type == TK_SCOPE_OPERATOR)
                            curFunction.signature += "::";
                        else
                            curFunction.signature += String(tokens->tokens[i].lexeme);
                    }

                    i++;
                }
            }
            else if ((token.type == TK_CE_FUNCTION || token.type == TK_CE_EVENT) && curClass != nullptr)
            {
                FunctionInfo curFunction{};
                int attribScope = 0;
                String curAttrib = "";
                i++;

                if (token.type == TK_CE_EVENT)
                {
                    curFunction.isEvent = true;
                    curFunction.attribs.Add("Event");
                }

                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE && attribScope <= 1)
                    {
                        i++;
                        break;
                    }
                    if (tokens->tokens[i].type == TK_PAREN_OPEN)
                    {
                        attribScope++;
                        if (attribScope > 1)
                            curAttrib += "(";
                    }
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                    {
                        if (attribScope > 1)
                            curAttrib += ")";
                        attribScope--;
                    }
                    if (attribScope == 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        if (!curAttrib.IsEmpty())
                            curFunction.attribs.Add(curAttrib);
                        curAttrib = "";
                    }
                    else if (attribScope > 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        curAttrib += ",";
                    }
                    else if (attribScope >= 1)
                    {
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        curAttrib += String(tokens->tokens[i].lexeme);
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                    }
                    i++;
                }

                if (!curAttrib.IsEmpty())
                {
                    curFunction.attribs.Add(curAttrib);
                    curAttrib = "";
                }

                attribScope = 0;
                int parenScope = 0;
                int funcBodyScope = 0;

                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_SCOPE_OPEN)
                    {
                        funcBodyScope++;
                    }
                    else if (tokens->tokens[i].type == TK_SCOPE_CLOSE)
                    {
                        funcBodyScope--;
                        if (funcBodyScope <= 0)
                            break;
                    }

                    if (funcBodyScope > 0)
                    {
                        i++;
                        continue;
                    }

                    if (parenScope == 0 && tokens->tokens[i].type == TK_KW_CONST)
                    {
                        curFunction.signature += " const";
                        curFunction.attribs.Add("Constant");
                    }

                    if (tokens->tokens[i].type == TK_PAREN_OPEN)
                    {
                        if (parenScope == 0)
                            curFunction.signature += "(";
                        parenScope++;
                        if (!curFunction.name.IsValid())
                            curFunction.name = String(tokens->tokens[i - 1].lexeme);

                        int backIdx = i - 1;
                        while (backIdx > 1)
                        {
                            
                            backIdx--;
                        }
                    }
                    else if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                    {
                        parenScope--;
                        if (parenScope == 0)
                            curFunction.signature += ")";
                    }
                    else if (tokens->tokens[i].type == TK_SEMICOLON)
                    {
                        break;
                    }
                    else if (parenScope > 0)
                    {
                        if (tokens->tokens[i].type == TK_COMMA)
                            curFunction.signature += ",";
                        else
                            curFunction.signature += String(tokens->tokens[i].lexeme) + " ";
                    }
                    
                    i++;
                }

                curClass->functions.Add(curFunction);
            }
            else if (token.type == TK_CE_FIELD && (curClass != nullptr || curStruct != nullptr))
            {
                auto structPtr = curStruct;
                if (structPtr == nullptr)
                    structPtr = curClass;
                FieldInfo curField{};
                int attribScope = 0;
                String curAttrib = "";
                i++;
                
                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE && attribScope <= 1)
                        break;
                    if (tokens->tokens[i].type == TK_PAREN_OPEN)
                    {
                        attribScope++;
                        if (attribScope > 1)
                            curAttrib += "(";
                    }
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                    {
                        if (attribScope > 1)
                            curAttrib += ")";
                        attribScope--;
                    }
                    if (attribScope == 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        if (!curAttrib.IsEmpty())
                            curField.attribs.Add(curAttrib);
                        curAttrib = "";
                    }
                    else if (attribScope > 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        curAttrib += ",";
                    }
                    else if (attribScope >= 1)
                    {
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        curAttrib += String(tokens->tokens[i].lexeme);
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                    }
                    i++;
                }
                
                if (!curAttrib.IsEmpty())
                {
                    curField.attribs.Add(curAttrib);
                    curAttrib = "";
                }
                
                attribScope = 0;
                
                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_SCOPE_OPEN)
                    {
                        attribScope++;
                    }
                    else if (tokens->tokens[i].type == TK_SCOPE_CLOSE)
                    {
                        attribScope--;
                        break;
                    }
                    else if (attribScope > 0)
                    {
                        i++;
                        continue;
                    }
                    if (tokens->tokens[i].type == TK_SCOPE_OPEN || tokens->tokens[i].type == TK_ASSIGNMENT_OPERATOR ||
                        tokens->tokens[i].type == TK_SEMICOLON)
                    {
                        curField.name = String(tokens->tokens[i - 1].lexeme);
                        structPtr->fields.Add(curField);
                        curField.name = "";
                        if (tokens->tokens[i].type == TK_SEMICOLON || tokens->tokens[i].type == TK_ASSIGNMENT_OPERATOR)
                            break;
                    }
                    i++;
                }
            }
            else if (token.type == TK_COLON)
            {
                if (curClass != nullptr && i > 0 && !curClass->name.IsValid())
                {
                    String nameSpace = "";
                    std::stack<NamespaceInfo> copy = curNamespace;
                    while (copy.size() > 0)
                    {
                        if (nameSpace.IsEmpty())
                            nameSpace = copy.top().name.GetString();
                        else
                            nameSpace = copy.top().name.GetString() + "::" + nameSpace;
                        copy.pop();
                    }
                    curClass->nameSpace = nameSpace;
                    curClass->name = String(tokens->tokens[i - 1].lexeme);
                }
                else if (curStruct != nullptr && i > 0 && !curStruct->name.IsValid())
                {
                    String nameSpace = "";
                    std::stack<NamespaceInfo> copy = curNamespace;
                    while (copy.size() > 0)
                    {
                        if (nameSpace.IsEmpty())
                            nameSpace = copy.top().name.GetString();
                        else
                            nameSpace = copy.top().name.GetString() + "::" + nameSpace;
                        copy.pop();
                    }
                    curStruct->nameSpace = nameSpace;
                    curStruct->name = String(tokens->tokens[i - 1].lexeme);
                }
            }
            else if (token.type == TK_CE_ENUM)
            {
                delete curEnum;
                curEnum = new EnumInfo;
                int attribScope = 0;
                String curAttrib = "";
                EnumConstInfo curConst{};
                i++;

                String nameSpace = "";
                std::stack<NamespaceInfo> copy = curNamespace;
                while (copy.size() > 0)
                {
                    if (nameSpace.IsEmpty())
                        nameSpace = copy.top().name.GetString();
                    else
                        nameSpace = copy.top().name.GetString() + "::" + nameSpace;
                    copy.pop();
                }
                curEnum->nameSpace = nameSpace;

                while (i < size)
                {
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE && attribScope <= 1)
                    {
                        i++;
                        break;
                    }
                    if (tokens->tokens[i].type == TK_PAREN_OPEN)
                    {
                        attribScope++;
                        if (attribScope > 1)
                            curAttrib += "(";
                    }
                    if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                    {
                        if (attribScope > 1)
                            curAttrib += ")";
                        attribScope--;
                    }
                    if (attribScope == 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        if (!curAttrib.IsEmpty())
                            curEnum->attribs.Add(curAttrib);
                        curAttrib = "";
                    }
                    else if (attribScope > 1 && tokens->tokens[i].type == TK_COMMA)
                    {
                        curAttrib += ",";
                    }
                    else if (attribScope >= 1)
                    {
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        curAttrib += String(tokens->tokens[i].lexeme);
                        if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                            curAttrib += "\'";
                        if (tokens->tokens[i].type == TK_LITERAL_STRING)
                            curAttrib += "\"";
                    }
                    i++;
                }

                if (!curAttrib.IsEmpty())
                {
                    curEnum->attribs.Add(curAttrib);
                    curAttrib = "";
                }

                attribScope = 0;
                bool insideEnum = false;
                bool shouldBreak = false;

                while (i < size && !shouldBreak)
                {

                    if ((tokens->tokens[i].type == TK_COLON || tokens->tokens[i].type == TK_SCOPE_OPEN) && !curEnum->name.IsValid())
                    {
                        int backIdx = i - 1;
                        while (backIdx > 0 && !curEnum->name.IsValid())
                        {
                            if (tokens->tokens[backIdx].type == TK_IDENTIFIER)
                            {
                                curEnum->name = String(tokens->tokens[backIdx].lexeme);
                                break;
                            }
                            backIdx--;
                        }
                    }

                    if (tokens->tokens[i].type == TK_SCOPE_OPEN)
                    {
                        insideEnum = true;
                    }
                    if (tokens->tokens[i].type == TK_SCOPE_CLOSE)
                    {
                        insideEnum = false;
                        i++;
                        ast->enums.Add(*curEnum);
                        delete curEnum;
                        curEnum = nullptr;
                        break;
                    }

                    attribScope = 0;
                    curAttrib = "";

                    // ECONST
                    if (tokens->tokens[i].type == TK_CE_ECONST)
                    {
                        curConst = {};
                        i++;

                        while (i < size)
                        {
                            if (tokens->tokens[i].type == TK_PAREN_CLOSE && attribScope <= 1)
                            {
                                i++;
                                if (!curAttrib.IsEmpty())
                                {
                                    curConst.attribs.Add(curAttrib);
                                    curAttrib = "";
                                }
                                break;
                            }
                            if (tokens->tokens[i].type == TK_PAREN_OPEN)
                            {
                                attribScope++;
                                if (attribScope > 1)
                                    curAttrib += "(";
                            }
                            if (tokens->tokens[i].type == TK_PAREN_CLOSE)
                            {
                                if (attribScope > 1)
                                    curAttrib += ")";
                                attribScope--;
                            }
                            if (attribScope == 1 && tokens->tokens[i].type == TK_COMMA)
                            {
                                if (!curAttrib.IsEmpty())
                                {
                                    curConst.attribs.Add(curAttrib);
                                    curAttrib = "";
                                }
                                curAttrib = "";
                            }
                            else if (attribScope > 1 && tokens->tokens[i].type == TK_COMMA)
                            {
                                curAttrib += ",";
                            }
                            else if (attribScope >= 1)
                            {
                                if (tokens->tokens[i].type == TK_LITERAL_STRING)
                                    curAttrib += "\"";
                                if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                                    curAttrib += "\'";
                                curAttrib += String(tokens->tokens[i].lexeme);
                                if (tokens->tokens[i].type == TK_LITERAL_CHAR)
                                    curAttrib += "\'";
                                if (tokens->tokens[i].type == TK_LITERAL_STRING)
                                    curAttrib += "\"";
                            }
                            i++;
                        }
                    }

                    // Identifier
                    if (tokens->tokens[i].type == TK_IDENTIFIER && insideEnum)
                    {
                        const auto& nameToken = tokens->tokens[i];
                        i++;

                        while (i < size)
                        {
                            if (tokens->tokens[i].type == TK_COMMA && curConst.name.IsValid())
                            {
                                curEnum->constants.Add(curConst);
                                curConst = {};
                                break;
                            }

                            if (tokens->tokens[i].type == TK_SCOPE_CLOSE)
                            {
                                shouldBreak = true;
                                if (!curConst.name.IsValid())
                                {
                                    curConst.name = String(nameToken.lexeme);
                                }
                                curEnum->constants.Add(curConst);
                                curConst = {};
                                ast->enums.Add(*curEnum);
                                delete curEnum;
                                curEnum = nullptr;
                                break;
                            }

                            if (!curConst.name.IsValid() && 
                                (tokens->tokens[i].type == TK_ASSIGNMENT_OPERATOR || tokens->tokens[i].type == TK_COMMA || tokens->tokens[i].type == TK_SCOPE_CLOSE))
                            {
                                curConst.name = String(nameToken.lexeme);
                                curEnum->constants.Add(curConst);
                                curConst = {};
                                if (tokens->tokens[i].type == TK_SCOPE_CLOSE)
                                {
                                    shouldBreak = true;
                                    ast->enums.Add(*curEnum);
                                    delete curEnum;
                                    curEnum = nullptr;
                                    break;
                                }

                                while (tokens->tokens[i].type != TK_COMMA)
                                {
                                    if (tokens->tokens[i].type == TK_SCOPE_CLOSE)
                                    {
                                        shouldBreak = true;
                                        ast->enums.Add(*curEnum);
                                        delete curEnum;
                                        curEnum = nullptr;
                                        break;
                                    }
                                    i++;
                                }
                                break;
                            }

                            i++;
                        }
                    }

                    i++;
                }
            }
            else if (token.type == TK_IDENTIFIER)
            {
                if (namespaceNameInProgress) // Namespace name being captured
                {
                    NamespaceInfo namespaceInfo{};
                    namespaceInfo.name = String(token.lexeme);
                    namespaceInfo.curScope = curScope;
                    curNamespace.push(namespaceInfo);
                }
            }
        }
        
        return ast;
    }

} // namespace CE

