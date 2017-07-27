#include "code_manager.h"

namespace tez {

Code_Manager::Code_Manager(Ast& ast)
        _ast{ast} {
}

int Code_Manager::errors_reported() const {
    return _errors_reported;
}

int Code_Manager::warnings_reported() const {
    return _warnings_reported;
}

}

