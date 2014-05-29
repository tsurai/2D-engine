#ifndef COMMON_H
#define COMMON_H

#include "prerequisites.h"
#include "typeinfo"

namespace Tsuki {
    #define UNUSED(x) (void)(x)
    #define READ_ONLY(type,varname) private: type varname##_m; public: inline type varname() { return varname##_m; };

    typedef std::map<const char *, Any> ParamList;

    class DynBuffer {
    public:
        DynBuffer(unsigned int size, unsigned short threshold, float growFactor, float shrinkFactor=0.0)
            : mData(malloc(size)), mThreshold(threshold),
              mGrow(growFactor), mShrink((!shrinkFactor) ? growFactor : shrinkFactor), counter(0) { }
        ~DynBuffer() { if(mData) free(mData); }

        inline const void *getData() { return mData; }
        inline int getSize() { return sizeof(mData); }
        inline void setGrowFactor(float factor) { if(factor > 1.0) mGrow = factor; }
        inline void setShrinkFactor(float factor) { if(factor < 1.0) mShrink = factor; }
        inline void setThreshold(unsigned short threshold) { mThreshold = threshold; }
        const void *requestData(unsigned int size) {
            if(size != sizeof(mData)) {
                if(size > sizeof(mData)) {
                    mData = realloc(mData, size*mGrow);
                }/* else if(++counter == mThreshold) {
                    mData = realloc(mData, (sizeof(mData)-size)*mShrink);
                    counter = 0;
                }*/
            }
            return mData;
        }
    private:
        void *mData;
        unsigned short mThreshold;
        float mGrow;
        float mShrink;
        int counter;
    };

    /* Any generic type class */
    class Any {
    public:
        /* constructor & destructor */
        Any() : content(0) { }

        template<typename T>
        Any(const T &value) : content(new Holder<T>(value)) { }

        Any(const Any & other) : content(other.content ? other.content->clone() : 0) { }

        ~Any() { delete content; }

        /* modifier */
        Any & swap(Any &other) {
            std::swap(content, other.content);
            return *this;
        }

        template<typename T>
        Any & operator=(const T &other) {
            Any(other).swap(*this);
            return *this;
        }

        Any & operator=(Any other) {
            other.swap(*this);
            return *this;
        }

        /* queries */
        template<typename T>
        const T *to_ptr() const {
            return this->type() == typeid(T)
                    ? &static_cast<Holder<T> *>(content)->value : 0;
        }

        bool emtpy() const {
            return !content;
        }

        const std::type_info &type() const {
            return content ? content->type() : typeid(void);
        }
    private:
        class Placeholder {
        public:
            virtual ~Placeholder() { }
            virtual const std::type_info &type() const = 0;
            virtual Placeholder *clone() const = 0;
        };

        template <typename T>
        class Holder : public Placeholder {
        public:
            Holder(const T &value) : value(value) { }

            virtual const std::type_info &type() const {
                return typeid(T);
            }

            virtual Placeholder *clone() const {
                return new Holder(value);
            }
            const T value;
        private:
            Holder & operator=(const Holder &);
        };
        Placeholder *content;
    };

    /* custom cast keywords */
    template<typename T>
    T any_cast(const Any &operand) {
        const T *result = operand.to_ptr<T>();
        return result ? *result : throw std::bad_cast();
    }

    template<typename T>
    T *any_cast(Any *operand) {
        return operand && operand->type() == typeid(T)
                ? &static_cast<Any::Holder<T> *>(operand->content)->value : 0;
    }

    template<typename T>
    inline const T * any_cast(const Any * operand) {
        return any_cast<T>(const_cast<Any *>(operand));
    }

    template<typename T_result, typename T_arg>
    T_result interpret_cast(const T_arg &arg) {
        std::stringstream interpreter;
        T_result result = T_result();

        interpreter << arg;
        interpreter >> result;

        return result;
    }

    class ForkStream: public std::ostream
    {
    public:
        ForkStream(std::ostream &os, const char *filename, ios_base::openmode mode = ios_base::app)
            : std::ostream(&buffer), buffer(os, *(new std::ofstream(filename, mode))) { }
        virtual ~ForkStream() { buffer.pubsync(); }

        void setFileOutEnabled(bool enable) {
            buffer.setFileOutEnabled(enable);
        }

        void setStrOutEnabled(bool enable) {
            buffer.setStrOutEnabled(enable);
        }
    protected:
        std::ostream &write(const char_type *__s, std::streamsize __n);
        std::ostream &put(char_type __c);
        std::streampos tellp();
        std::ostream seekp(off_type, ios_base::seekdir);
        std::ostream seekp(std::streampos);

        class ForkStringBuf: public std::stringbuf
        {
        public:
            ForkStringBuf(std::ostream& os, std::ofstream &fos) : out(os), fout(fos), fOut(true), strOut(true) { }
            ~ForkStringBuf() { }

            void setFileOutEnabled(bool enable) {
                fOut = enable;
            }

            void setStrOutEnabled(bool enable) {
                strOut = enable;
            }
        protected:
            virtual int sync()
            {
                if(str().empty())
                    return 0;

                if(strOut) {
                    out << str();
                    out.flush();
                }
                if(fOut) {
                    fout << str();
                    fout.flush();
                }
                str("");
                return 0;
            }
            std::ostream &out;
            std::ofstream &fout;
            bool fOut;
            bool strOut;
        };
        ForkStringBuf buffer;
    };
}

#endif // COMMON_H
