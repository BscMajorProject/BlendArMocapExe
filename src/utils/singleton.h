// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
namespace BlendArMocap
{
    class Singleton
    {
        public:
            static Singleton& getInstance()
            {
                static Singleton   instance; // Guaranteed to be destroyed.
                                      // Instantiated on first use.
                return instance;
            }
        private:
            Singleton() {}                    // Constructor? (the {} brackets) are needed here.
    
            // C++ 03
            // ========
            // Don't forget to declare these two. You want to make sure they
            // are inaccessible(especially from outside), otherwise, you may accidentally get copies of
            // your singleton appearing.
            Singleton(Singleton const&);              // Don't Implement
            void operator=(Singleton const&); // Don't implement
    
            // C++ 11
            // =======
            // We can use the better technique of deleting the methods
            // we don't want.
        public:
            Singleton(Singleton const&) = delete;
            void operator=(Singleton const&) = delete;
    
            // Note: Scott Meyers mentions in his Effective Modern
            //       C++ book, that deleted functions should generally
            //       be public as it results in better error messages
            //       due to the compilers behavior to check accessibility
            //       before deleted status
    };
}
