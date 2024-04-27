#pragma once

namespace CE
{

    template<typename Func>
    class JobFunction : public Job
    {
        CE_NO_COPY(JobFunction)
    public:
        
        JobFunction(const Func& func, bool isAutoDelete = true, JobContext* context = nullptr);
        
        virtual ~JobFunction();

        void Process() override;

    private:

        Func function;
    };
    
    template<typename Func>
    JobFunction<Func>::JobFunction(const Func& func, bool isAutoDelete, JobContext* context) : Job(isAutoDelete, context), function(func)
    {
        
    }
    
    template<typename Func>
    JobFunction<Func>::~JobFunction()
    {
        
    }
    
    template<typename Func>
    void JobFunction<Func>::Process()
    {
        function(this);
    }

} // namespace CE
