#include <iostream>
#include <string>
#include <stack>
#include <Windows.h>
#include "tinyxml\include\tinyxml\tinystr.h"
#include "tinyxml\include\tinyxml\tinyxml.h"

using namespace std;


class V {
public:
  //V的类型的枚举：Vt、Vn
  typedef enum V_type {
    Vt = 1,
    Vn = 2,
    empty = 0,
    other = -1
  } Vtype;
  V() { }
  V(string data, Vtype vtype) {
    this->data = data;
    this->vtype = vtype;
  }
  //运算符==重载，单目运算符，判断相等
  bool operator== (const V &v) {
    if (v.data == this->data && v.vtype == this->vtype) {
      return true;
    }
    return false;
  }
  //类型
  Vtype vtype;
  //V的名称
  string data;
  //指向xml节点的指针
  TiXmlElement *xml_ptr = NULL;
};

//V+，若干个V组成的
class Vplus {    //TODO: 所有类的析构函数，delete
public:
  Vplus() {
    len = 0;
    data = data = new V[100];
  }
  Vplus(int length) {
    len = length;
    data = new V[length];
  }
  //重载操作符=，方便复制
  Vplus operator= (const Vplus &v_plus) {
    len = v_plus.len;
    for (int i = 0; i < len; i++) {
      data[i] = v_plus.data[i];
    }
    return *this;
  }
  //指向V数组的指针
  V *data;
  int len = 0;
};
typedef Vplus FirstSet, FollowSet;


//产生式，由左部右部组成，左部右部都是若干个V+
class Production {
public:
  Production() {
    this->left_len = 0;
    this->right_len = 0;
    left = NULL;
    right = NULL;
  }
  Production(int left_len, int right_len) {
    this->left_len = left_len;
    this->right_len = right_len;
    left = new Vplus[left_len];
    right = new Vplus[right_len];
  }
  Production Error() {
    Production p;
    p.left_len = -1;
    return p;
  }
  //左部，指向V+数组的指针
  Vplus *left;
  //右部，指向V+数组的指针
  Vplus *right;
  int left_len = 1;
  int right_len;
};

//文法，由(Vn, Vt, S, P)四元组组成
class Grammar {
public:
  Grammar() {
    this->production_amount = 0;
    production = NULL;
    this->vt_amount = 0;
    v_t = NULL;
    this->vn_amount = 0;
    v_n = NULL;
  }
  Grammar(int production_amount, int vt_amount, int vn_amount) {
    this->production_amount = production_amount;
    production = new Production[production_amount];
    this->vt_amount = vt_amount;
    v_t = new V[vt_amount];
    this->vn_amount = vn_amount;
    v_n = new V[vn_amount];
  }
  //P，产生式集合
  Production *production;
  int production_amount;
  //S，开始符号
  V v_start;
  //Vt，终结符号集
  V *v_t;
  int vt_amount;
  //Vn，非终结符号集
  V *v_n;
  int vn_amount;
};

//根据txt文本中的V的string data部分，在语法中的VtVn数组中查找，并返回type。(vt/vn/empty)
V::Vtype ReturnTypeOfV(string str, const Grammar &grammar) {
  for (int i = 0; i < grammar.vn_amount; i++) {
    if (str == grammar.v_n[i].data) {
      return V::Vtype::Vn;
    }
  }
  for (int i = 0; i < grammar.vt_amount; i++) {
    if (str == grammar.v_t[i].data && str != "/e") {
      return V::Vtype::Vt;
    }
  }
  if (str == "/e") {
    return V::Vtype::empty;
  }
  return V::Vtype::other;
}


//根据Vn，在文法中找出以Vn为左部的表达式并返回
//出错则返回left_len == -1的Production对象的引用
Production ReturnProductionOfVn(const V &v, const Grammar &grammar) {
  if (v.vtype != V::Vtype::Vn) {
    Production p;
    return p.Error();
  }
  for (int i = 0; i < grammar.production_amount; i++) {
    if (grammar.production[i].left[0].data[0] == v) {
      return grammar.production[i];
    }
  }
  Production p;
  return p.Error();
}

//根据Vn，在文法中找出以Vn为左部的表达式在grammar中的index并返回（从0开始）
//出错则返回-1
int ReturnProductionIndexOfVn(const V &v, const Grammar &grammar) {
  if (v.vtype != V::Vtype::Vn) {
    return -1;
  }
  for (int i = 0; i < grammar.production_amount; i++) {
    if (grammar.production[i].left[0].data[0] == v) {
      return i;
    }
  }
  return -1;
}

//根据Vt，在文法中找出vt在grammar中v_t[]的index并返回（从0开始）
//出错则返回-1
int ReturnVtIndexOfVt(const V &v, const Grammar &grammar) {
  if (v.vtype != V::Vtype::Vt) {
    return -1;
  }
  for (int i = 0; i < grammar.vt_amount; i++) {
    if (grammar.v_t[i] == v) {
      return i;
    }
  }
  return -1;
}

//根据Vn，在文法中找出vn在grammar中v_n[]的index并返回（从0开始）
//出错则返回-1
int ReturnVnIndexOfVn(const V &v, const Grammar &grammar) {
  if (v.vtype != V::Vtype::Vn) {
    return -1;
  }
  for (int i = 0; i < grammar.vn_amount; i++) {
    if (grammar.v_n[i] == v) {
      return i;
    }
  }
  return -1;
}

//将两个V[] 求并集
FirstSet Union(FirstSet set1, FirstSet set2) {
  //如果set1为空
  if (set1.len == 0) {
    return set2;
  }
  //set1非空
  FirstSet set(set1.len + set2.len);  //TODO
  set.len = set1.len;
  for (int i = 0; i < set1.len; i++) {
    set.data[i] = set1.data[i];
  }
  for (int i = 0; i < set2.len; i++) {
    for (int j = 0; j < set1.len; j++) {
      if (set2.data[i] == set1.data[j]) {
        break;
      }
      if (j == set1.len - 1) {
        set.data[set.len] = set2.data[i];
        set.len++;
      }
    }
  }
  return set;
}


//将V[] 减去空串
FirstSet SubtractEpsilon(FirstSet set) {
  FirstSet result(set.len);  //TODO
  result.len = set.len;
  for (int i = 0; i < result.len; i++) {
    result.data[i] = set.data[i];
  }
  for (int i = 0; i < result.len; i++) {
    if (result.data[i].vtype == V::Vtype::empty) {
      for (int j = i + 1; j < result.len; j++) {
        result.data[j - 1] = result.data[j];
      }
      result.len--;
      return result;
    }
  }
  return result;
}

//查看V[] 内是否有空串
bool IfExistEpsilon(FirstSet set) {
  for (int i = 0; i < set.len; i++) {
    if (set.data[i] == V("/e", V::Vtype::empty)) {
      return true;
    }
  }
  return false;
}

//计算V+的First集
FirstSet First(const Vplus &v_plus, const Grammar &grammar) {
  //如果是空串ε
  if (v_plus.data[0].vtype == V::V_type::empty) {
    return v_plus;
  }
  FirstSet result(200);  //保存结果
  result.len = 0;
  for (int i = 0; i < v_plus.len; i++) {
    //如果是终结符，直接添加至first集，结束
    if (v_plus.data[i].vtype == V::V_type::Vt) {
      Vplus tmp(1);
      tmp.data[0] = V(v_plus.data[i]);
      result = Union(tmp, result);
      return result;
    //如果是非终结符
    } else if (v_plus.data[i].vtype == V::V_type::Vn){
     // 找到非终结符的产生式
      Production production = ReturnProductionOfVn(v_plus.data[i], grammar);
      //对每个产生式右部的V+，求他们的first并集
      for (int j = 0; j < production.right_len; j++) {
        result = Union(result, First(production.right[j], grammar));
      }
      //如果没有epsilon，则return
      if (!IfExistEpsilon(result)) {
        return result;
      //如果有，则删掉，继续。（如果都计算过了还有，则最后再补上一个epsilon
      } else {
        result = SubtractEpsilon(result);
      }
    }
  }
  //补上一个epsilon
  Vplus tmp(1);
  tmp.data[0] = V("/e", V::Vtype::empty);
  result = Union(result, tmp);
  return result;
}

//计算文法中所有Vn的Follow集
//返回值是FollowSet[]的基指针
//followset是根据[Vn在grammar.v_n中的存储index]
FollowSet* Follow(const Grammar &grammar) {
  //给vn_amount个Followset开空间
  FollowSet *followset_ptr = new FollowSet[grammar.vn_amount];
  //找到开始符号S在grammar.vn中的位置，填入#
  int s_index = ReturnProductionIndexOfVn(grammar.v_start, grammar);
  followset_ptr[s_index].len = 1;
  followset_ptr[s_index].data[0] = V("#", V::Vtype::Vt);
  //反复应用两条规则
  bool changed = true;
  //循环直到followset无改变
  while (changed == true) {
    changed = false;
    for (int vn_index = 0; vn_index < grammar.vn_amount; vn_index++) {
      for (int i = 0; i < grammar.production_amount; i++) {
        for (int j = 0; j < grammar.production[i].right_len; j++) {
          for (int k = 0; k < grammar.production[i].right[j].len; k++) {
            if (grammar.production[i].right[j].data[k] == grammar.v_n[vn_index]) {
              //如果beta不是空（即vn不是V+中的最后一个）
              if (k != grammar.production[i].right[j].len - 1) {
                Vplus beta;
                for (int n = k + 1; n < grammar.production[i].right[j].len; n++) {
                  beta.data[n - k - 1] = grammar.production[i].right[j].data[n];
                }
                beta.len = grammar.production[i].right[j].len - k - 1;
                int original_len = followset_ptr[vn_index].len;
                //将除去epsilon的first(beta)加入followset
                followset_ptr[vn_index] = Union(followset_ptr[vn_index], SubtractEpsilon(First(beta, grammar)));
                if (followset_ptr[vn_index].len != original_len) {  //记录变化
                  changed = true;
                }
                //若first(beta)有epsilon，则follow(左部)需要加入followset
                if (IfExistEpsilon(First(beta, grammar))) {
                  int original_len = followset_ptr[vn_index].len;
                  //获取左部的index
                  int left_part_vindex = ReturnVnIndexOfVn(grammar.production[i].left[0].data[0], grammar);
                  //follow(左部) 加入followset
                  followset_ptr[vn_index] = Union(followset_ptr[vn_index], followset_ptr[left_part_vindex]);
                  if (followset_ptr[vn_index].len != original_len) {  //记录变化
                    changed = true;
                  }
                }
              //如果beta是空（即vn是V+中的最后一个）
              } else {
                //和first(beta)有epsilon的情况一样的操作
                int original_len = followset_ptr[vn_index].len;
                //获取左部的index
                int left_part_vindex = ReturnVnIndexOfVn(grammar.production[i].left[0].data[0], grammar);
                //follow(左部) 加入followset
                followset_ptr[vn_index] = Union(followset_ptr[vn_index], followset_ptr[left_part_vindex]);
                if (followset_ptr[vn_index].len != original_len) {  //记录变化
                  changed = true;
                }
              }
            }
          }
        }
      }
    }
  }
  return followset_ptr;
}

//建立LL1分析表，返回二维数组指针
Vplus** CreateLL1_Table(const Grammar &grammar) {
  //分配LL1分析表的二维数组空间
  Vplus** ll1_table = new Vplus*[grammar.production_amount];
  for (int i = 0; i < grammar.production_amount; i++) {
    ll1_table[i] = new Vplus[grammar.vt_amount + 1];
  }
  FollowSet * followset = Follow(grammar);
  for (int i = 0; i < grammar.vn_amount; i++) {
    int production_index = ReturnProductionIndexOfVn(grammar.v_n[i], grammar);
    for (int j = 0; j < grammar.production[production_index].right_len; j++) {
      //对每个a属于first(ri)-epsilon, 置table[vn_index][a的vt_index]为ri
      FirstSet firstset = First(grammar.production[production_index].right[j], grammar);
      FirstSet tmp = SubtractEpsilon(firstset);
      for (int k = 0; k < SubtractEpsilon(firstset).len; k++) {
        ll1_table[i][ReturnVtIndexOfVt(SubtractEpsilon(firstset).data[k], grammar)] = grammar.production[production_index].right[j];
      }
      //如果epsilon属于first(ri)，则对所有的a属于follow（左部），还要置table[左部index][a的vt_index]为ri
      if (IfExistEpsilon(firstset)) {
        
        for (int k = 0; k < followset[i].len; k++) {
          //如果是#，index为v_t[]的len
          if (followset[i].data[k].data == "#") {
            ll1_table[i][grammar.vt_amount] = grammar.production[production_index].right[j];
          //除了#
          } else {
            ll1_table[i][ReturnVtIndexOfVt(followset[i].data[k], grammar)] = grammar.production[production_index].right[j];
          }
        }
      }
    }
  }
  return ll1_table;
}

//LL1分析器主控程序
void LL1_Analizer( Vplus** ptr_LL1_table, Grammar &grammar, const Vplus &inputstr) {
  std::stack<V> stack;
  stack.push(V("#", V::Vtype::Vt));
  stack.push(grammar.v_start);
  Vplus input;
  input = inputstr;
  input.data[input.len] = V("#", V::Vtype::Vt);  //输入串最后添加#
  input.len++;
  //xml头的输出
  const char * xmlFile = ".\\tree_output.xml";	
	TiXmlDocument doc;  
	TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "UTF-8", "");
	TiXmlElement * titleElement = new TiXmlElement( "ParserTree" );  
  titleElement->SetAttribute("name", "tree_output.xml");
	doc.LinkEndChild(decl);  
	doc.LinkEndChild(titleElement); 
  TiXmlElement * root = new TiXmlElement("TreeRoot");
	doc.LinkEndChild(root);
  grammar.v_start.xml_ptr = root;
  grammar.v_n[0].xml_ptr = root;
  while (true) {
    if (input.data[0].vtype == V::Vtype::Vt && stack.top().vtype == V::Vtype::Vt) {  //如果栈顶是Vt
      //如果栈顶 == 串开头 == ‘#’，成功
      if ((input.data[0] == V("#", V::Vtype::Vt) && stack.top() == V("#", V::Vtype::Vt))) {
        printf("success\n");
        break;
      //如果栈顶 == 串开头 ！= ‘#’，弹出两个符号
      } else if (input.data[0] == stack.top()) {
        stack.pop();
        input.data = &input.data[1];
      } else {   //其他情况错误
        printf("error!\n");
        break;
      }
    } else {    //栈顶是Vn的话
      int vnindex = ReturnProductionIndexOfVn(stack.top(), grammar);
      int vtindex = ReturnVtIndexOfVt(input.data[0], grammar);
      if (ptr_LL1_table[vnindex][vtindex].len == 0) {
        printf("error!\n");
        break;
      } else {
        for (int i = 0; i < ptr_LL1_table[vnindex][vtindex].len; i++) {
        //查LL1分析表，并将栈顶弹出，将表中的产生式右部逆序入栈，同时记录xml节点指针，输出xml树
          int index;
          if (ptr_LL1_table[vnindex][vtindex].data[i].vtype == V::V_type::Vn) {
            index = ReturnProductionIndexOfVn(ptr_LL1_table[vnindex][vtindex].data[i], grammar);
            //if (grammar.v_n[index].xml_ptr == NULL) {
              TiXmlElement * xmlnode = new TiXmlElement((ptr_LL1_table[vnindex][vtindex].data[i].data).data());
              grammar.v_n[index].xml_ptr = xmlnode;
            //}
            int stacktop_vnindex = ReturnProductionIndexOfVn(stack.top(), grammar);
            grammar.v_n[stacktop_vnindex].xml_ptr->LinkEndChild(grammar.v_n[index].xml_ptr);
          } else if (ptr_LL1_table[vnindex][vtindex].data[i].vtype == V::V_type::Vt) {
            index = ReturnVtIndexOfVt(ptr_LL1_table[vnindex][vtindex].data[i], grammar);
            //if (grammar.v_t[index].xml_ptr == NULL) {
              TiXmlElement * xmlnode = new TiXmlElement((ptr_LL1_table[vnindex][vtindex].data[i].data).data());
              grammar.v_t[index].xml_ptr = xmlnode;
            //}
            int stacktop_vnindex = ReturnProductionIndexOfVn(stack.top(), grammar);
            grammar.v_n[stacktop_vnindex].xml_ptr->LinkEndChild(grammar.v_t[index].xml_ptr);
          }
          
        }
        stack.pop();
        for (int i = ptr_LL1_table[vnindex][vtindex].len - 1; i >= 0; i--) {
          if (!(ptr_LL1_table[vnindex][vtindex].data[i] == V("/e", V::Vtype::empty)))
          stack.push(ptr_LL1_table[vnindex][vtindex].data[i]);
        }
      }
    }
  }
  
  doc.SaveFile(xmlFile);  
  return;
}




//从写有文法的txt文件中读取文法，存在grammar对象中
Grammar ReadGrammarFromTXT() {
  FILE *fp;
  fp = fopen("grammar.txt", "r");

  Grammar grammar(1000, 100, 100);
  grammar.vn_amount = 0;
  grammar.vt_amount = 0;
  grammar.production_amount = 0;
  char buffer[1005];
  while (!feof(fp)) {
    fgets(buffer, 1005, fp); //按行读取txt
    //如果行首是S:，读开始符号
    if (strstr(buffer, "S:") != NULL) {
      printf("S:\n");
      int tmplen = strlen(buffer);
      //将' '和'\n'替换为'\0'
      for (int i = 0; i < tmplen; i++) {
        if (buffer[i] == ' ' || buffer[i] == '\n') {
          buffer[i] = '\0';
        }
      }
      grammar.v_start = V(&buffer[3], V::Vtype::Vn);
    //如果行首是Vt:，读终结符
    } else if (strstr(buffer, "Vt:") != NULL) {
      printf("Vt\n");
      int tmplen = strlen(buffer);
      //将' '和'\n'替换为'\0'
      for (int i = 0; i < tmplen; i++) {
        if (buffer[i] == ' ' || buffer[i] == '\n') {
          buffer[i] = '\0';
        }
      }
      for (int i = 4; i < tmplen - 1; ) {
        printf("%d:\n", grammar.vt_amount);
        if (buffer[i] != '/')
          grammar.v_t[grammar.vt_amount] = V(&buffer[i], V::Vtype::Vt);
        else
          grammar.v_t[grammar.vt_amount] = V(&buffer[i], V::Vtype::empty);
        i += strlen(&buffer[i]) + 1;
        grammar.vt_amount++;
      }
    //如果行首是Vn:，读非终结符
    } else if (strstr(buffer, "Vn:") != NULL) {
      printf("Vn\n");
      int tmplen = strlen(buffer);
      //将' '和'\n'替换为'\0'
      for (int i = 0; i < tmplen; i++) {
        if (buffer[i] == ' ' || buffer[i] == '\n') {
          buffer[i] = '\0';
        }
      }
      for (int i = 4; i < tmplen - 1; ) {
        printf("%d:\n", grammar.vn_amount);
        grammar.v_n[grammar.vn_amount] = V(&buffer[i], V::Vtype::Vn);
        i += strlen(&buffer[i]) + 1;
        grammar.vn_amount++;
      }
    //读产生式
    } else {
      printf("production\n");
      //将' '和'\n'替换为'\0'
      int tmplen = strlen(buffer);
      for (int i = 0; i < tmplen; i++) {
        if (buffer[i] == ' ' || buffer[i] == '\n') {
          buffer[i] = '\0';
        }
      }
      bool if_right_part = false;  // 标记要读取的V+是左部还是右部
      grammar.production[grammar.production_amount].left_len = 0;
      grammar.production[grammar.production_amount].right_len = 0;
      //给left和right的Vplus对象分配内存
      grammar.production[grammar.production_amount].left = new Vplus[100];
      grammar.production[grammar.production_amount].right = new Vplus[100];

      for (int i = 0; i < tmplen - 1; ) {
        //如果是第一个V+，是左部
        if (i == 0) {
          int currentpos = grammar.production[grammar.production_amount].left_len;
          grammar.production[grammar.production_amount].left[currentpos] = Vplus(1);
          grammar.production[grammar.production_amount].left[currentpos].data[0] = V(&buffer[i], ReturnTypeOfV(&buffer[i], grammar));
          grammar.production[grammar.production_amount].left_len++;
          i += strlen(&buffer[i]) + 1;
        //如果读的是->
        } else if (strcmp(&buffer[i], "->") == 0) {
          if_right_part = true;
          i += strlen(&buffer[i]) + 1;
          //新建第一个右部的Vplus的工作在这里完成
          int currentpos = grammar.production[grammar.production_amount].right_len;
          grammar.production[grammar.production_amount].right[currentpos] = Vplus(100);
          grammar.production[grammar.production_amount].right[currentpos].len = 0;
          
          continue;
        } else {
          //如果读的是左部
          if (!if_right_part) {
            int currentpos = grammar.production[grammar.production_amount].left_len;
            grammar.production[grammar.production_amount].left[currentpos] = Vplus(1);  //TODO: 这里左部固定了只有一个 V
            grammar.production[grammar.production_amount].left[currentpos].data[0] = V(&buffer[i], ReturnTypeOfV(&buffer[i], grammar));
            grammar.production[grammar.production_amount].left_len++;
            i += strlen(&buffer[i]) + 1;
          //如果读的是右部
          //如果不是 |
          } else if (strcmp(&buffer[i], "|") != 0) {
            int currentpos = grammar.production[grammar.production_amount].right_len;
            int v_data_pos = grammar.production[grammar.production_amount].right[currentpos].len;
            grammar.production[grammar.production_amount].right[currentpos].data[v_data_pos] = V(&buffer[i], ReturnTypeOfV(&buffer[i], grammar));
            grammar.production[grammar.production_amount].right[currentpos].len++;
            i += strlen(&buffer[i]) + 1;
          //如果是 |
          } else {
            //新建右部的Vplus的工作在这里完成
            i += strlen(&buffer[i]) + 1;
            grammar.production[grammar.production_amount].right_len++;  // 将右部的V+数量+1
            int currentpos = grammar.production[grammar.production_amount].right_len;
            grammar.production[grammar.production_amount].right[currentpos] = Vplus(100);
            grammar.production[grammar.production_amount].right[currentpos].len = 0;
          }
        }
      }
      grammar.production[grammar.production_amount].right_len++;  // 将右部的V+数量+1
      grammar.production_amount++;
    }
  }
  if (fp != NULL) {
    fclose(fp);
  }
  grammar.production_amount--;
  return grammar;
}




int main() {
  //读取文法
  Grammar grammar = ReadGrammarFromTXT();  //txt文件最后一行一定要是空行
  //创建LL1分析表
  Vplus** ptr_to_LL1_table = CreateLL1_Table(grammar);
  //读取输入串（这里没有用xml输入）
  Vplus input(18);
  input.data[0] = V("type-specifier", V::Vtype::Vt);
  input.data[1] = V("identifier", V::Vtype::Vt);
  input.data[2] = V("=", V::Vtype::Vt);
  input.data[3] = V("constant", V::Vtype::Vt);
  input.data[4] = V(";", V::Vtype::Vt);
  input.data[5] = V("type-specifier", V::Vtype::Vt);
  input.data[6] = V("identifier", V::Vtype::Vt);
  input.data[7] = V("=", V::Vtype::Vt);
  input.data[8] = V("constant", V::Vtype::Vt);
  input.data[9] = V(";", V::Vtype::Vt);
  input.data[10] = V("while", V::Vtype::Vt);
  input.data[11] = V("(", V::Vtype::Vt);
  input.data[12] = V("constant", V::Vtype::Vt);
  input.data[13] = V(")", V::Vtype::Vt);
  input.data[14] = V("identifier", V::Vtype::Vt);
  input.data[15] = V("=", V::Vtype::Vt);
  input.data[16] = V("identifier", V::Vtype::Vt);
  input.data[17] = V(";", V::Vtype::Vt);
  //运行LL1总控程序，输出xml格式语法分析树
  LL1_Analizer(ptr_to_LL1_table, grammar, input);

  system("pause");
  return 0;
}
//-----------------------------------
//测试程序段：
//int a = 1;
//int b = 2;
//while(true)
//  b = a;
//---------------------------------
