

namespace CE
{

    class CORE_API Gradient
    {
    public:
        struct Key
        {
			Color value{};
            f32 position{};

			SIZE_T GetHash() const
			{
				SIZE_T hash = value.GetHash();
				CombineHash(hash, position);
				return hash;
			}
        };
        
        Gradient();
		Gradient(std::initializer_list<Key> list, f32 degrees = 0);
		Gradient(const Array<Key>& list, f32 degrees = 0);

		f32 GetDegrees() const
		{
			return degrees;
		}

		void SetDegrees(f32 degrees)
		{
			this->degrees = degrees;
		}

		u32 GetNumKeys() const
		{
			return keys.GetSize();
		}

		const Array<Key>& GetKeys() const { return keys; }

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

		SIZE_T GetHash() const;

		bool operator==(const Gradient& rhs) const
		{
			return GetHash() == rhs.GetHash();
		}

		bool operator!=(const Gradient& rhs) const
		{
			return !operator==(rhs);
		}

    private:

        Array<Key> keys{};

		f32 degrees = 0.0f;
    };
    
} // namespace CE



