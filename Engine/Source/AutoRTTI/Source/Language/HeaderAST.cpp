
#include "HeaderAST.h"

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
        int curClassScope;
        
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
                if (namespaceNameInProgress)
                {
                    namespaceNameInProgress = false;
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
            else if (token.type == TK_PAREN_OPEN)
            {
                curParenthesis++;
            }
            else if (token.type == TK_PAREN_CLOSE)
            {
                curParenthesis--;
                curClassScope = false;
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
            else if (token.type == TK_CE_FIELD && curClass != nullptr)
            {
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
                        curClass->fields.Add(curField);
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
                            nameSpace = nameSpace + "::" + copy.top().name.GetString();
                        copy.pop();
                    }
                    curClass->nameSpace = nameSpace;
                    curClass->name = String(tokens->tokens[i - 1].lexeme);
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

