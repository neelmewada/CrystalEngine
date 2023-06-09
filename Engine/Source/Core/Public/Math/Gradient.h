

namespace CE
{

    class CORE_API Gradient
    {
    public:
        struct Key
        {
            f32 position{};
            Color value{};
        };

		enum Direction
		{
			LeftToRight = 0,
			TopToBottom
		};
        
        Gradient();
        Gradient(std::initializer_list<Key> list, Direction dir = LeftToRight);
        
		Direction GetDirection() const { return direction; }

		void SetDirection(Direction direction)
		{
			this->direction = direction;
		}

		u32 GetNumKeys() const
		{
			return keys.GetSize();
		}

		const Key& GetKeyAt(u32 index) const
		{
			return keys[index];
		}

		Key& GetKeyAt(u32 index)
		{
			return keys[index];
		}

		Color Evaluate(f32 position) const;

		void Clear();

		void AddKey(const Key& key);

		void AddKey(f32 position, const Color& color);

		void RemoveKeyAt(u32 index);

    private:

        Array<Key> keys{};

		Direction direction{};
    };
    
} // namespace CE



