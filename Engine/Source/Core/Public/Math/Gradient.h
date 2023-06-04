

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
        
        Gradient();
        Gradient(std::initializer_list<Key> list);
        
    private:
        Array<Key> keys{};
    };
    
} // namespace CE

