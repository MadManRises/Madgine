#include "../interfaceslib.h"

#include "traceback.h"

namespace Engine{
    namespace Util{

        std::ostream &operator <<(std::ostream &out, const TraceBack &trace){
            out << trace.mFunction;
            if (!trace.mFile.empty()){
                out << " { " << trace.mFile;
                if (trace.mLineNr != -1)
                    out << " [" << trace.mLineNr << "]";
                out << " } ";
            }
            return out;
        }

    }
}