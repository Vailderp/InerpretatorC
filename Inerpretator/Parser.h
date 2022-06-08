#pragma once
#include <map>
#include <string>
#include <fstream>
#include <list>
#include <xmemory>
#include <iostream>
#include <vector>
#include <type_traits>


namespace prs
{
	namespace _priv
	{
		inline void __fastcall skip_space(const char** ptr_content)
		{
			while (**ptr_content == ' ' || **ptr_content == '\n' || **ptr_content == '\t')
			{
				(*ptr_content)++;
			}
		}
	}

	template <typename _FlagU32>
	inline constexpr const uint32_t __fastcall make_flag(_FlagU32 flag)
	{
		static_assert(
			std::is_arithmetic_v<_FlagU32> || std::is_enum_v<_FlagU32>, 
			"_FlagU32 is not arithmetic type;"
			"_FlagU32 is not enum type;"
			);
		return static_cast<uint32_t>(flag);
	}

	template <typename _FlagU32, typename... _FlagsU32>
	inline constexpr const uint32_t __fastcall make_flag(_FlagU32 flag, _FlagsU32... flags)
	{
		static_assert(
			std::is_arithmetic_v<_FlagU32> || std::is_enum_v<_FlagU32>,
			"_FlagU32 is not arithmetic type;"
			"_FlagU32 is not enum type;"
			);
		return static_cast<uint32_t>(flag) | make_flag<_FlagsU32...>(flags...);
	}

	template <typename _CompU32, typename _FlagU32>
	inline constexpr const bool __fastcall comp_flag(
		_CompU32 comp, 
		uint32_t ignore_mask, 
		_FlagU32 flag
	)
	{
		static_assert(
			(std::is_arithmetic_v<_FlagU32> || std::is_enum_v<_FlagU32>) &&
			(std::is_arithmetic_v<_CompU32> || std::is_enum_v<_CompU32>),
			"_FlagU32 is not arithmetic type;"
			"_FlagU32 is not enum type;"
			"_CompU32 is not arithmetic type;"
			"_CompU32 is not enum type;"
			);
		return ((static_cast<uint32_t>(comp) & ignore_mask) == (static_cast<uint32_t>(flag)));
	}

	template <typename _CompU32, typename _FlagU32, typename... _FlagsU32>
	inline constexpr const bool __fastcall comp_flag(
		_CompU32 comp, 
		uint32_t ignore_mask, 
		_FlagU32 flag,  
		_FlagsU32... flags
	)
	{
		static_assert(
			(std::is_arithmetic_v<_FlagU32> || std::is_enum_v<_FlagU32>) && 
			(std::is_arithmetic_v<_CompU32> || std::is_enum_v<_CompU32>),
			"_FlagU32 is not arithmetic type;"
			"_FlagU32 is not enum type;"
			"_CompU32 is not arithmetic type;"
			"_CompU32 is not enum type;"
			);
		return ((static_cast<uint32_t>(comp) & ignore_mask) == (static_cast<uint32_t>(flag)))
			&& comp_flag(comp, flags...);
	}

	enum class EDefinitionTraits : uint32_t
	{
		Numric,
		Character,
		String,
		Array,
		Expression
	};

	enum class ENumericTypeTraits : uint32_t
	{
		SChar =		0x10000000,
		UChar =		0x20000000,
		SShort =	0x30000000,
		UShort =	0x40000000,
		SInt =		0x50000000,
		UInt =		0x60000000,
		Float =		0x70000000,
		SLong =		0x80000000,
		ULong =		0x90000000,
		SLLong =	0xA0000000,
		ULLong =	0xB0000000,
		Double =	0xC0000000,
	};

	class Lexeme;
	class ExpressionLexeme;
	class DefinitionTokenStructure;
	class DefinitionTokenStructureDictionaryTreesQueue;

	class ParserAllocator
	{
	public:
		Lexeme* createLexeme(const char* chars, const uint32_t length)
		{
			Lexeme* lexeme = m_lexeme_allocator.allocate(1);
			m_lexeme_allocator.construct(lexeme, chars, length);
			m_lexeme_pointers.push_back(lexeme);
			return lexeme;
		}

		Lexeme* createLexeme(const char* chars)
		{
			Lexeme* lexeme = m_lexeme_allocator.allocate(1);
			m_lexeme_allocator.construct(lexeme, chars);
			m_lexeme_pointers.push_back(lexeme);
			return lexeme;
		}

		Lexeme* createExpressionLexeme(const char* chars, const uint32_t length);

		Lexeme* createExpressionLexeme(const char* chars);

		void deleteLexeme(Lexeme* lexeme)
		{
			m_lexeme_allocator.deallocate(lexeme, 1);
		}

		void deleteExpressionLexeme(Lexeme* lexeme)
		{
			m_expr_lexeme_allocator.deallocate((ExpressionLexeme*)lexeme, 1);
		}

		template <typename... _String_lexemes>
		DefinitionTokenStructure* createDefinitionTokenStructure(
			const uint32_t user_data, 
			_String_lexemes... lexemes
		)
		{
			DefinitionTokenStructure* def_token_struct = m_def_token_struct_allocator.allocate(1);
			m_def_token_struct_allocator.construct(def_token_struct, this, user_data, lexemes...);
			m_def_token_struct_pointers.push_back(def_token_struct);
			return def_token_struct;
		}

		void deleteDefinitionTokenStructure(DefinitionTokenStructure* def_token_struct)
		{
			m_def_token_struct_allocator.deallocate(def_token_struct, 1);
		}

		template <typename... _Trees>
		DefinitionTokenStructureDictionaryTreesQueue* createDefinitionTokenStructureDictionaryTreesQueue(_Trees*... trees)
		{
			DefinitionTokenStructureDictionaryTreesQueue* def_token_struct_dict_trees_queue = m_def_token_struct_dict_trees_queue_allocator.allocate(1);
			m_def_token_struct_dict_trees_queue_allocator.construct(def_token_struct_dict_trees_queue, trees...);
			m_def_token_struct_dict_trees_queue_pointers.push_back(def_token_struct_dict_trees_queue);
			return def_token_struct_dict_trees_queue;
		}

		void deleteDefinitionTokenStructureDictionaryTreesQueue(DefinitionTokenStructureDictionaryTreesQueue* def_token_struct_dict_trees_queue)
		{
			m_def_token_struct_dict_trees_queue_allocator.deallocate(def_token_struct_dict_trees_queue, 1);
		}

	private:

		std::allocator<Lexeme> m_lexeme_allocator;
		std::list<Lexeme*> m_lexeme_pointers;

		std::allocator<ExpressionLexeme> m_expr_lexeme_allocator;

		std::allocator<DefinitionTokenStructure>  m_def_token_struct_allocator;
		std::list<DefinitionTokenStructure*> m_def_token_struct_pointers;

		std::allocator<DefinitionTokenStructureDictionaryTreesQueue>  m_def_token_struct_dict_trees_queue_allocator;
		std::list<DefinitionTokenStructureDictionaryTreesQueue*> m_def_token_struct_dict_trees_queue_pointers;

	};

	class Lexeme
	{
	public:
		Lexeme() = default;

		Lexeme(const char* chars, const uint32_t length) :
			m_chars(chars),
			m_length(length),
			m_ull_length((m_length + static_cast<const uint32_t>(sizeof(const uint32_t))) 
				/ static_cast<const uint32_t>(sizeof(const uint32_t)))
		{

		}

		Lexeme(const char* chars) :
			m_chars(chars),
			m_length(static_cast<uint32_t>(strlen(chars))),
			m_ull_length((m_length + static_cast<const uint32_t>(sizeof(const uint32_t))) 
				/ static_cast<const uint32_t>(sizeof(const uint32_t)))
		{

		}

		bool operator == (const Lexeme& lexeme) const
		{
			if (this->m_length != lexeme.m_length || 
				this->m_chars == nullptr || lexeme.m_chars == nullptr)
			{
				return false;
			}
			for (uint32_t i = 0; i < this->m_ull_length; i++)
			{
				if (reinterpret_cast<const uint32_t*>(this->m_chars)[i] != 
					reinterpret_cast<const uint32_t*>(lexeme.m_chars)[i])
				{
					return false;
				}
			}
			return true;
		}

		bool operator != (const Lexeme& lexeme) const
		{
			if (*this == lexeme)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		bool operator < (const Lexeme& lexeme) const
		{
			uint32_t offset = 0;
			if (this->m_length < lexeme.m_length)
			{
				return false;
			}
			else if (this->m_length > lexeme.m_length)
			{
				return true;
			}
			while (this->m_chars[offset] == lexeme.m_chars[offset])
			{
				offset++;
				if (offset == this->m_length)
				{
					break;
				}
				if (offset == lexeme.m_length)
				{
					break;
				}
			}
			if (this->m_chars[offset] < lexeme.m_chars[offset])
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool compareStrict(const char* chars, const uint32_t length) const
		{
			if (this->m_length != length || this->m_chars == nullptr
				|| chars == nullptr)
			{
				return false;
			}
			for (uint32_t i = 0; i < this->m_ull_length; i++)
			{
				if (reinterpret_cast<const uint32_t*>(this->m_chars)[i] != 
					reinterpret_cast<const uint32_t*>(chars)[i])
				{
					return false;
				}
			}
			return true;
		}

		bool compareStrict(const char* chars) const
		{
			return this->compareStrict(chars, strlen(chars));
		}

		virtual bool compare(const char* chars, const uint32_t length) const
		{
			if (this->m_length != length || this->m_chars == nullptr 
				|| chars == nullptr)
			{
				return false;
			}
			for (uint32_t i = 0; i < this->m_length; i++)
			{
				if (this->m_chars[i] != chars[i])
				{
					return false;
				}
			}
			return true;
		}

		bool compare(const char* chars) const
		{
			return this->compare(chars, strlen(chars));
		}

		virtual uint32_t getLength(const char* content) const
		{
			return m_length;
		}

		const char* getChars() const
		{
			return m_chars;
		}

	protected:
		uint32_t m_length = 0;
		uint32_t m_ull_length = 0;
		const char* m_chars = nullptr;
	};

	class ExpressionLexeme : public Lexeme
	{
	public:
		ExpressionLexeme() = default;

		ExpressionLexeme(const char* chars, const uint32_t length) :
			Lexeme(chars, length)
		{

		}

		ExpressionLexeme(const char* chars) :
			Lexeme(chars)
		{

		}

		bool compare(const char* chars, const uint32_t length) const override
		{
			if (this->m_chars == nullptr || chars == nullptr)
			{
				return false;
			}
			for (uint32_t i = 0; i < length; i++)
			{
				bool finded = false;
				for (uint32_t l = 0; l < this->m_length; l++)
				{
					if (chars[i] == this->m_chars[l])
					{
						finded = true;
						break;
					}
				}
				if (!finded)
				{
					return false;
				}
			}
			return true;
		}

		uint32_t getLength(const char* content) const override
		{
			uint32_t lexeme_length = 0;
			while (content[lexeme_length] != '\0')
			{
				bool finded = false;
				for (uint32_t i = 0; i < this->m_length; i++)
				{
					if (content[lexeme_length] == this->m_chars[i])
					{
						finded = true;
						break;
					}
				}
				if (!finded)
				{
					break;
				}
				lexeme_length++;
			}
			return lexeme_length;
		}
	};

	inline Lexeme* prs::ParserAllocator::createExpressionLexeme(const char* chars, const uint32_t length)
	{
		ExpressionLexeme* lexeme = m_expr_lexeme_allocator.allocate(1);
		m_expr_lexeme_allocator.construct(lexeme, chars, length);
		m_lexeme_pointers.push_back(lexeme);
		return lexeme;
	}

	inline Lexeme* ParserAllocator::createExpressionLexeme(const char* chars)
	{
		ExpressionLexeme* lexeme = m_expr_lexeme_allocator.allocate(1);
		m_expr_lexeme_allocator.construct(lexeme, chars);
		m_lexeme_pointers.push_back(lexeme);
		return lexeme;
	}

	class LexemeComparer
	{
	public:
		LexemeComparer() = default;

		LexemeComparer(const Lexeme* ptr_lexeme) : 
			m_ptr_lexeme(ptr_lexeme)
		{

		}

		bool operator == (const LexemeComparer& lexeme_comp) const
		{
			return *this->m_ptr_lexeme == *lexeme_comp.m_ptr_lexeme;
		}

		bool operator != (const LexemeComparer& lexeme_comp) const
		{
			return *this->m_ptr_lexeme != *lexeme_comp.m_ptr_lexeme;
		}

		bool operator < (const LexemeComparer& lexeme_comp) const
		{
			return *this->m_ptr_lexeme < *lexeme_comp.m_ptr_lexeme;
		}

		const Lexeme* getLexeme() const
		{
			return m_ptr_lexeme;
		}

	private:
		const Lexeme* m_ptr_lexeme = nullptr;
	};

	class DefinitionTokenStructure
	{
	public:
		DefinitionTokenStructure() : 
			m_lexemes(nullptr),
			m_lexemes_count(0),
			m_user_data(0)
		{

		}

		~DefinitionTokenStructure()
		{
			delete[] m_lexemes;
		}

	private:
		template <typename _String_lexeme>
		void create(
			ParserAllocator* ptr_allocator, 
			const uint32_t offset, 
			_String_lexeme lexeme)
		{
			if (lexeme[0] == '$')
			{
				m_lexemes[offset] = ptr_allocator->createExpressionLexeme(&lexeme[1]);
			}
			else
			{
				m_lexemes[offset] = ptr_allocator->createLexeme(lexeme);
			}
		}

		template <typename _String_lexeme, typename... _String_lexemes>
		void create(
			ParserAllocator* ptr_allocator,
			const uint32_t offset, 
			_String_lexeme lexeme, 
			_String_lexemes... lexemes
		)
		{
			if (lexeme[0] == '$')
			{
				m_lexemes[offset] = ptr_allocator->createExpressionLexeme(&lexeme[1]);
			}
			else
			{
				m_lexemes[offset] = ptr_allocator->createLexeme(lexeme);
			}
			this->create<_String_lexemes...>(ptr_allocator, offset + 1, lexemes...);
		}

	public:
		template <typename _String_lexeme, typename... _String_lexemes>
		DefinitionTokenStructure(
			ParserAllocator* ptr_allocator,
			const uint32_t user_data,
			_String_lexeme lexeme, 
			_String_lexemes... lexemes
		) :
			m_lexemes_count(sizeof...(_String_lexemes) + 1),
			m_lexemes(new const Lexeme*[m_lexemes_count]),
			m_user_data(user_data)
		{
			this->create<_String_lexeme, _String_lexemes...>(ptr_allocator, 0, lexeme, lexemes...);
		}

		bool compare(const char* content) const
		{
			for (uint32_t i = 0; i < m_lexemes_count; i++)
			{
				const uint32_t lexeme_length = m_lexemes[i]->getLength(content);
				if (!m_lexemes[i]->compare(content, lexeme_length))
				{
					return false;
				}
				content += lexeme_length;
			}
			return true;
		}

		uint32_t getLength(const char* content) const
		{
			uint32_t length = 0;
			for (uint32_t i = 0; i < m_lexemes_count; i++)
			{
				const uint32_t lexeme_length = m_lexemes[i]->getLength(content);
				length += lexeme_length;
				content += lexeme_length;
			}
			return length;
		}

		const Lexeme* getLexemeAt(const uint32_t index) const
		{
			return m_lexemes[index];
		}

		const uint32_t getLexemesCount() const
		{
			return m_lexemes_count;
		}

		const uint32_t getUserData() const
		{
			return m_user_data;
		}

		const uint32_t m_lexemes_count;
		const Lexeme** m_lexemes;
		const uint32_t m_user_data;
	};

	class DefinitionTokenStructureDictionaryTree;

	class DefinitionTokenStructureDictionaryTreeNode
	{

		friend class DefinitionTokenStructureDictionaryTree;

	public:
		DefinitionTokenStructureDictionaryTreeNode() = default;
		

		DefinitionTokenStructure* getDefinitionTokenStructure() const
		{
			return m_def_token_struct;
		}

		bool definitionTokenStructureEmpty() const
		{
			return m_def_token_struct == nullptr;
		}

		const std::map<LexemeComparer, std::vector<DefinitionTokenStructureDictionaryTreeNode*>>& getNext() const
		{
			return m_next;
		}

		bool nextEmpty() const
		{
			return m_next.empty();
		}

	private:
		DefinitionTokenStructure* m_def_token_struct = nullptr;
		std::map<LexemeComparer, std::vector<DefinitionTokenStructureDictionaryTreeNode*>> m_next;
	};

	class DefinitionTokenStructureDictionaryTree
	{
	public:
		DefinitionTokenStructureDictionaryTree() = default;

		void pushDefinitionTokenStructure(DefinitionTokenStructure* ptr_def_tok_struct)
		{
			const uint32_t lexemes_count = ptr_def_tok_struct->getLexemesCount();
			DefinitionTokenStructureDictionaryTreeNode* next_node = &m_node;
			for (uint32_t i = 0; i < lexemes_count; i++)
			{
				const Lexeme* key_lexeme = ptr_def_tok_struct->getLexemeAt(i);
				std::vector<DefinitionTokenStructureDictionaryTreeNode*>& defs = next_node->m_next[LexemeComparer(key_lexeme)];
				defs.push_back(new DefinitionTokenStructureDictionaryTreeNode());
				next_node = defs.back();
			}
			next_node->m_def_token_struct = ptr_def_tok_struct;
		}

		void pushDefinitionTokenStructures(std::vector<DefinitionTokenStructure*> ptr_def_tok_structs)
		{
			for (auto ptr_def_tok_struct : ptr_def_tok_structs)
			{
				pushDefinitionTokenStructure(ptr_def_tok_struct);
			}
		}

		void findByChars(
			const char* chars, 
			DefinitionTokenStructure** pptr_def_tok_struct,
			DefinitionTokenStructureDictionaryTreeNode& node
		)
		{
			if (node.m_next.empty())
			{
				if (chars[0] == ' ' || chars[0] == '\0' || chars[0] == '\n' || chars[0] == '\t')
				{
					if (*pptr_def_tok_struct == nullptr)
					{
						*pptr_def_tok_struct = node.m_def_token_struct;
					}
					else if (node.m_def_token_struct != nullptr)
					{
						if (node.m_def_token_struct->getLexemesCount() > (*pptr_def_tok_struct)->getLexemesCount())
						{
							*pptr_def_tok_struct = node.m_def_token_struct;
						}
					}
				}
			}
			for (decltype(auto) pair : node.m_next)
			{
				const Lexeme* lexeme = pair.first.getLexeme();
				const uint32_t lexeme_length = lexeme->getLength(chars);
				if (lexeme == nullptr)
				{
					return;
				}
				else if (lexeme->compare(chars, lexeme_length))
				{
					for (int i = 0 ; i < pair.second.size(); i++)
					{
						findByChars(&chars[lexeme_length], pptr_def_tok_struct, *pair.second[i]);
					}
				}
			}
		}

		void findByChars(
			const char* chars,
			DefinitionTokenStructure** pptr_def_tok_struct
		)
		{
			if (*chars == ' ' || *chars == '\n' || *chars == '\t' || *chars == '\0')
			{
				return;
			}
			findByChars(chars, pptr_def_tok_struct, m_node);
		}

	private:
		DefinitionTokenStructureDictionaryTreeNode m_node;
	};

	class DefinitionTokenStructureDictionaryTrees
	{
	private:
		template <typename... _String_lexemes>
		void add(
			DefinitionTokenStructureDictionaryTree& tree,
			const uint32_t user_data, 
			_String_lexemes... lexemes
		)
		{
			tree.pushDefinitionTokenStructure(m_allocator.createDefinitionTokenStructure(user_data, lexemes...));
		}

	public:
		DefinitionTokenStructureDictionaryTrees(ParserAllocator& allocator) : 
			m_allocator(allocator)
		{
			constexpr uint32_t float_type_flag = make_flag(ENumericTypeTraits::Float);
			constexpr uint32_t double_type_flag = make_flag(ENumericTypeTraits::Double);
			constexpr uint32_t int_type_flag = make_flag(ENumericTypeTraits::SInt);

			this->add(tree_bkt_figure_open, NULL, "{");
			this->add(tree_bkt_figure_close, NULL, "}");
			this->add(tree_assignment, NULL, "=");
			this->add(tree_semicolon, NULL, ";");

			this->add(tree_variable_name, NULL, "$_qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890");

			this->add(tree_type, NULL, "const", "$ ", "int");
			this->add(tree_type, NULL, "const", "$ ", "float");
			this->add(tree_type, NULL, "const", "$ ", "double");
			this->add(tree_type, NULL, "const", "$ ", "long");

			this->add(tree_numeric, float_type_flag, "+", "$0123456789", ".", "$0123456789", "f");
			this->add(tree_numeric, float_type_flag, "+", "$0123456789", ".", "$0123456789", "F");
			this->add(tree_numeric, float_type_flag, "+", "$0123456789", ".", "f");
			this->add(tree_numeric, float_type_flag, "+", "$0123456789", ".", "F");
			this->add(tree_numeric, float_type_flag, "-", "$0123456789", ".", "$0123456789", "f");
			this->add(tree_numeric, float_type_flag, "-", "$0123456789", ".", "$0123456789", "F");
			this->add(tree_numeric, float_type_flag, "-", "$0123456789", ".", "f");
			this->add(tree_numeric, float_type_flag, "-", "$0123456789", ".", "F");
			this->add(tree_numeric, float_type_flag, "$0123456789", ".", "$0123456789", "f");
			this->add(tree_numeric, float_type_flag, "$0123456789", ".", "$0123456789", "F");
			this->add(tree_numeric, float_type_flag, "$0123456789", ".", "f");
			this->add(tree_numeric, float_type_flag, "$0123456789", ".", "F");
			this->add(tree_numeric, float_type_flag, "-", ".", "$0123456789", "f");
			this->add(tree_numeric, float_type_flag, "-", ".", "$0123456789", "F");
			this->add(tree_numeric, float_type_flag, "+", ".", "$0123456789", "f");
			this->add(tree_numeric, float_type_flag, "+", ".", "$0123456789", "F");
			this->add(tree_numeric, float_type_flag, ".", "$0123456789", "f");
			this->add(tree_numeric, float_type_flag, ".", "$0123456789", "F");

			this->add(tree_numeric, int_type_flag, "+", "$0123456789");
			this->add(tree_numeric, int_type_flag, "-", "$0123456789");
			this->add(tree_numeric, int_type_flag, "$0123456789");

			this->add(tree_numeric, double_type_flag, "+", "$0123456789", ".", "$0123456789", "d");
			this->add(tree_numeric, double_type_flag, "+", "$0123456789", ".", "$0123456789", "D");
			this->add(tree_numeric, double_type_flag, "+", "$0123456789", ".", "d");
			this->add(tree_numeric, double_type_flag, "+", "$0123456789", ".", "D");
			this->add(tree_numeric, double_type_flag, "-", "$0123456789", ".", "$0123456789", "d");
			this->add(tree_numeric, double_type_flag, "-", "$0123456789", ".", "$0123456789", "D");
			this->add(tree_numeric, double_type_flag, "-", "$0123456789", ".", "d");
			this->add(tree_numeric, double_type_flag, "-", "$0123456789", ".", "D");
			this->add(tree_numeric, double_type_flag, "$0123456789", ".", "$0123456789", "d");
			this->add(tree_numeric, double_type_flag, "$0123456789", ".", "$0123456789", "D");
			this->add(tree_numeric, double_type_flag, "$0123456789", ".", "d");
			this->add(tree_numeric, double_type_flag, "$0123456789", ".", "D");
			this->add(tree_numeric, double_type_flag, "-", ".", "$0123456789", "d");
			this->add(tree_numeric, double_type_flag, "-", ".", "$0123456789", "D");
			this->add(tree_numeric, double_type_flag, "+", ".", "$0123456789", "d");
			this->add(tree_numeric, double_type_flag, "+", ".", "$0123456789", "D");
			this->add(tree_numeric, double_type_flag, ".", "$0123456789", "d");
			this->add(tree_numeric, double_type_flag, ".", "$0123456789", "D");
			this->add(tree_numeric, double_type_flag, "+", "$0123456789", ".", "$0123456789");
			this->add(tree_numeric, double_type_flag, "+", "$0123456789", ".");
			this->add(tree_numeric, double_type_flag, "-", "$0123456789", ".", "$0123456789");
			this->add(tree_numeric, double_type_flag, "-", "$0123456789", ".");
			this->add(tree_numeric, double_type_flag, "$0123456789", ".", "$0123456789");
			this->add(tree_numeric, double_type_flag, "$0123456789", ".");
			this->add(tree_numeric, double_type_flag, "-", ".", "$0123456789");
			this->add(tree_numeric, double_type_flag, "+", ".", "$0123456789");
			this->add(tree_numeric, double_type_flag, ".", "$0123456789");
		}

		DefinitionTokenStructureDictionaryTree tree_type;
		DefinitionTokenStructureDictionaryTree tree_variable_name;
		DefinitionTokenStructureDictionaryTree tree_numeric;
		DefinitionTokenStructureDictionaryTree tree_bkt_figure_open;
		DefinitionTokenStructureDictionaryTree tree_bkt_figure_close;
		DefinitionTokenStructureDictionaryTree tree_semicolon;
		DefinitionTokenStructureDictionaryTree tree_assignment;

	private:
		ParserAllocator& m_allocator;
	};

	class DefinitionTokenStructureDictionaryTreesQueue
	{
	public:
		DefinitionTokenStructureDictionaryTreesQueue() = default;

		template <typename _Tree, typename... _Trees>
		DefinitionTokenStructureDictionaryTreesQueue(_Tree* tree, _Trees*... trees) : 
			m_length(sizeof...(_Trees) + 1),
			m_trees(new DefinitionTokenStructureDictionaryTree*[m_length])
		{
			this->create(0, tree, trees...);
		}

		uint32_t getLength() const
		{
			return m_length;
		}

		DefinitionTokenStructureDictionaryTree* getTreeAt(uint32_t index) const
		{
			return m_trees[index];
		}

		std::vector<uint32_t> compare(const char** ptr_content) const
		{
			std::vector<uint32_t> user_data_queue;
			for (uint32_t i = 0; i < m_length; i++)
			{
				_priv::skip_space(ptr_content);
				DefinitionTokenStructure* def_tok_struct = nullptr;
				m_trees[i]->findByChars(*ptr_content, &def_tok_struct);
				if (def_tok_struct == nullptr)
				{
					return {};
				}
				user_data_queue.push_back(def_tok_struct->getUserData());
				*ptr_content += def_tok_struct->getLength(*ptr_content);
			}
			return user_data_queue;
		}

	private:
		template <typename _Tree>
		void create(const uint32_t offset, _Tree* tree)
		{
			static_assert(std::is_same_v<_Tree, DefinitionTokenStructureDictionaryTree>);
			static_assert(std::is_same_v<_Tree, DefinitionTokenStructureDictionaryTree>);
			m_trees[offset] = tree;
		}

		template <typename _Tree, typename... _Trees>
		void create(const uint32_t offset, _Tree* tree, _Trees*... trees)
		{
			static_assert(std::is_same_v<_Tree, DefinitionTokenStructureDictionaryTree>);
			m_trees[offset] = tree;
			this->create(offset + 1, trees...);
		}

		const uint32_t m_length;
		DefinitionTokenStructureDictionaryTree** m_trees;
	};

	class Parser
	{
	public:
		void fromFile(const std::string& file_path)
		{
			std::string file_content;
			std::getline(std::ifstream(file_path), file_content, '\0');
			fromMemory(file_content.data());

		}

		void fromMemory(const char* content)
		{
			ParserAllocator allocator;
			DefinitionTokenStructureDictionaryTrees trees(allocator);
			DefinitionTokenStructureDictionaryTreesQueue* queue = allocator.createDefinitionTokenStructureDictionaryTreesQueue(&trees.tree_type, &trees.tree_numeric);
			DefinitionTokenStructure* ptr_def_tok_struct = nullptr;	


			while (*content != '\0')
			{
				_priv::skip_space(&content);
				trees.tree_type.findByChars(content, &ptr_def_tok_struct);
				if (ptr_def_tok_struct != nullptr)
				{
					content += ptr_def_tok_struct->getLength(content);
					ptr_def_tok_struct = nullptr;
					trees.tree_variable_name.findByChars(content, &ptr_def_tok_struct);
					if (ptr_def_tok_struct != nullptr)
					{
						content += ptr_def_tok_struct->getLength(content);
						ptr_def_tok_struct = nullptr;
						trees.tree_assignment.findByChars(content, &ptr_def_tok_struct);
						if (ptr_def_tok_struct != nullptr)
						{
							content += ptr_def_tok_struct->getLength(content);
							ptr_def_tok_struct = nullptr;
							trees.tree_numeric.findByChars(content, &ptr_def_tok_struct);
							if (ptr_def_tok_struct != nullptr)
							{
								content += ptr_def_tok_struct->getLength(content);
								ptr_def_tok_struct = nullptr;
								trees.tree_semicolon.findByChars(content, &ptr_def_tok_struct);
								if (ptr_def_tok_struct != nullptr)
								{
									content += ptr_def_tok_struct->getLength(content);
									ptr_def_tok_struct = nullptr;
									std::cout << "good";
								}
							}
						}
					}
				}
			}
		
		}

	private:
		
	};
}