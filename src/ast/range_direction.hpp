
#ifndef AST_RANGEDIRECTION_H_
#define AST_RANGEDIRECTION_H_

namespace ast {
    
  class RangeDirection {

  public:

    enum Direction {
      DOWNTO,
      TO};

    Direction direction;
    
  };

}

#endif
