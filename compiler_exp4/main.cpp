#include <iostream>
#include <string>
#include <stack>
#include <Windows.h>
#include "tinyxml\include\tinyxml\tinystr.h"
#include "tinyxml\include\tinyxml\tinyxml.h"

using namespace std;


class V {
public:
  //V�����͵�ö�٣�Vt��Vn
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
  //�����==���أ���Ŀ��������ж����
  bool operator== (const V &v) {
    if (v.data == this->data && v.vtype == this->vtype) {
      return true;
    }
    return false;
  }
  //����
  Vtype vtype;
  //V������
  string data;
  //ָ��xml�ڵ��ָ��
  TiXmlElement *xml_ptr = NULL;
};

//V+�����ɸ�V��ɵ�
class Vplus {    //TODO: �����������������delete
public:
  Vplus() {
    len = 0;
    data = data = new V[100];
  }
  Vplus(int length) {
    len = length;
    data = new V[length];
  }
  //���ز�����=�����㸴��
  Vplus operator= (const Vplus &v_plus) {
    len = v_plus.len;
    for (int i = 0; i < len; i++) {
      data[i] = v_plus.data[i];
    }
    return *this;
  }
  //ָ��V�����ָ��
  V *data;
  int len = 0;
};
typedef Vplus FirstSet, FollowSet;


//����ʽ�������Ҳ���ɣ����Ҳ��������ɸ�V+
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
  //�󲿣�ָ��V+�����ָ��
  Vplus *left;
  //�Ҳ���ָ��V+�����ָ��
  Vplus *right;
  int left_len = 1;
  int right_len;
};

//�ķ�����(Vn, Vt, S, P)��Ԫ�����
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
  //P������ʽ����
  Production *production;
  int production_amount;
  //S����ʼ����
  V v_start;
  //Vt���ս���ż�
  V *v_t;
  int vt_amount;
  //Vn�����ս���ż�
  V *v_n;
  int vn_amount;
};

//����txt�ı��е�V��string data���֣����﷨�е�VtVn�����в��ң�������type��(vt/vn/empty)
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


//����Vn�����ķ����ҳ���VnΪ�󲿵ı��ʽ������
//�����򷵻�left_len == -1��Production���������
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

//����Vn�����ķ����ҳ���VnΪ�󲿵ı��ʽ��grammar�е�index�����أ���0��ʼ��
//�����򷵻�-1
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

//����Vt�����ķ����ҳ�vt��grammar��v_t[]��index�����أ���0��ʼ��
//�����򷵻�-1
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

//����Vn�����ķ����ҳ�vn��grammar��v_n[]��index�����أ���0��ʼ��
//�����򷵻�-1
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

//������V[] �󲢼�
FirstSet Union(FirstSet set1, FirstSet set2) {
  //���set1Ϊ��
  if (set1.len == 0) {
    return set2;
  }
  //set1�ǿ�
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


//��V[] ��ȥ�մ�
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

//�鿴V[] ���Ƿ��пմ�
bool IfExistEpsilon(FirstSet set) {
  for (int i = 0; i < set.len; i++) {
    if (set.data[i] == V("/e", V::Vtype::empty)) {
      return true;
    }
  }
  return false;
}

//����V+��First��
FirstSet First(const Vplus &v_plus, const Grammar &grammar) {
  //����ǿմ���
  if (v_plus.data[0].vtype == V::V_type::empty) {
    return v_plus;
  }
  FirstSet result(200);  //������
  result.len = 0;
  for (int i = 0; i < v_plus.len; i++) {
    //������ս����ֱ�������first��������
    if (v_plus.data[i].vtype == V::V_type::Vt) {
      Vplus tmp(1);
      tmp.data[0] = V(v_plus.data[i]);
      result = Union(tmp, result);
      return result;
    //����Ƿ��ս��
    } else if (v_plus.data[i].vtype == V::V_type::Vn){
     // �ҵ����ս���Ĳ���ʽ
      Production production = ReturnProductionOfVn(v_plus.data[i], grammar);
      //��ÿ������ʽ�Ҳ���V+�������ǵ�first����
      for (int j = 0; j < production.right_len; j++) {
        result = Union(result, First(production.right[j], grammar));
      }
      //���û��epsilon����return
      if (!IfExistEpsilon(result)) {
        return result;
      //����У���ɾ�����������������������˻��У�������ٲ���һ��epsilon
      } else {
        result = SubtractEpsilon(result);
      }
    }
  }
  //����һ��epsilon
  Vplus tmp(1);
  tmp.data[0] = V("/e", V::Vtype::empty);
  result = Union(result, tmp);
  return result;
}

//�����ķ�������Vn��Follow��
//����ֵ��FollowSet[]�Ļ�ָ��
//followset�Ǹ���[Vn��grammar.v_n�еĴ洢index]
FollowSet* Follow(const Grammar &grammar) {
  //��vn_amount��Followset���ռ�
  FollowSet *followset_ptr = new FollowSet[grammar.vn_amount];
  //�ҵ���ʼ����S��grammar.vn�е�λ�ã�����#
  int s_index = ReturnProductionIndexOfVn(grammar.v_start, grammar);
  followset_ptr[s_index].len = 1;
  followset_ptr[s_index].data[0] = V("#", V::Vtype::Vt);
  //����Ӧ����������
  bool changed = true;
  //ѭ��ֱ��followset�޸ı�
  while (changed == true) {
    changed = false;
    for (int vn_index = 0; vn_index < grammar.vn_amount; vn_index++) {
      for (int i = 0; i < grammar.production_amount; i++) {
        for (int j = 0; j < grammar.production[i].right_len; j++) {
          for (int k = 0; k < grammar.production[i].right[j].len; k++) {
            if (grammar.production[i].right[j].data[k] == grammar.v_n[vn_index]) {
              //���beta���ǿգ���vn����V+�е����һ����
              if (k != grammar.production[i].right[j].len - 1) {
                Vplus beta;
                for (int n = k + 1; n < grammar.production[i].right[j].len; n++) {
                  beta.data[n - k - 1] = grammar.production[i].right[j].data[n];
                }
                beta.len = grammar.production[i].right[j].len - k - 1;
                int original_len = followset_ptr[vn_index].len;
                //����ȥepsilon��first(beta)����followset
                followset_ptr[vn_index] = Union(followset_ptr[vn_index], SubtractEpsilon(First(beta, grammar)));
                if (followset_ptr[vn_index].len != original_len) {  //��¼�仯
                  changed = true;
                }
                //��first(beta)��epsilon����follow(��)��Ҫ����followset
                if (IfExistEpsilon(First(beta, grammar))) {
                  int original_len = followset_ptr[vn_index].len;
                  //��ȡ�󲿵�index
                  int left_part_vindex = ReturnVnIndexOfVn(grammar.production[i].left[0].data[0], grammar);
                  //follow(��) ����followset
                  followset_ptr[vn_index] = Union(followset_ptr[vn_index], followset_ptr[left_part_vindex]);
                  if (followset_ptr[vn_index].len != original_len) {  //��¼�仯
                    changed = true;
                  }
                }
              //���beta�ǿգ���vn��V+�е����һ����
              } else {
                //��first(beta)��epsilon�����һ���Ĳ���
                int original_len = followset_ptr[vn_index].len;
                //��ȡ�󲿵�index
                int left_part_vindex = ReturnVnIndexOfVn(grammar.production[i].left[0].data[0], grammar);
                //follow(��) ����followset
                followset_ptr[vn_index] = Union(followset_ptr[vn_index], followset_ptr[left_part_vindex]);
                if (followset_ptr[vn_index].len != original_len) {  //��¼�仯
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

//����LL1���������ض�ά����ָ��
Vplus** CreateLL1_Table(const Grammar &grammar) {
  //����LL1������Ķ�ά����ռ�
  Vplus** ll1_table = new Vplus*[grammar.production_amount];
  for (int i = 0; i < grammar.production_amount; i++) {
    ll1_table[i] = new Vplus[grammar.vt_amount + 1];
  }
  FollowSet * followset = Follow(grammar);
  for (int i = 0; i < grammar.vn_amount; i++) {
    int production_index = ReturnProductionIndexOfVn(grammar.v_n[i], grammar);
    for (int j = 0; j < grammar.production[production_index].right_len; j++) {
      //��ÿ��a����first(ri)-epsilon, ��table[vn_index][a��vt_index]Ϊri
      FirstSet firstset = First(grammar.production[production_index].right[j], grammar);
      FirstSet tmp = SubtractEpsilon(firstset);
      for (int k = 0; k < SubtractEpsilon(firstset).len; k++) {
        ll1_table[i][ReturnVtIndexOfVt(SubtractEpsilon(firstset).data[k], grammar)] = grammar.production[production_index].right[j];
      }
      //���epsilon����first(ri)��������е�a����follow���󲿣�����Ҫ��table[��index][a��vt_index]Ϊri
      if (IfExistEpsilon(firstset)) {
        
        for (int k = 0; k < followset[i].len; k++) {
          //�����#��indexΪv_t[]��len
          if (followset[i].data[k].data == "#") {
            ll1_table[i][grammar.vt_amount] = grammar.production[production_index].right[j];
          //����#
          } else {
            ll1_table[i][ReturnVtIndexOfVt(followset[i].data[k], grammar)] = grammar.production[production_index].right[j];
          }
        }
      }
    }
  }
  return ll1_table;
}

//LL1���������س���
void LL1_Analizer( Vplus** ptr_LL1_table, Grammar &grammar, const Vplus &inputstr) {
  std::stack<V> stack;
  stack.push(V("#", V::Vtype::Vt));
  stack.push(grammar.v_start);
  Vplus input;
  input = inputstr;
  input.data[input.len] = V("#", V::Vtype::Vt);  //���봮������#
  input.len++;
  //xmlͷ�����
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
    if (input.data[0].vtype == V::Vtype::Vt && stack.top().vtype == V::Vtype::Vt) {  //���ջ����Vt
      //���ջ�� == ����ͷ == ��#�����ɹ�
      if ((input.data[0] == V("#", V::Vtype::Vt) && stack.top() == V("#", V::Vtype::Vt))) {
        printf("success\n");
        break;
      //���ջ�� == ����ͷ ��= ��#����������������
      } else if (input.data[0] == stack.top()) {
        stack.pop();
        input.data = &input.data[1];
      } else {   //�����������
        printf("error!\n");
        break;
      }
    } else {    //ջ����Vn�Ļ�
      int vnindex = ReturnProductionIndexOfVn(stack.top(), grammar);
      int vtindex = ReturnVtIndexOfVt(input.data[0], grammar);
      if (ptr_LL1_table[vnindex][vtindex].len == 0) {
        printf("error!\n");
        break;
      } else {
        for (int i = 0; i < ptr_LL1_table[vnindex][vtindex].len; i++) {
        //��LL1����������ջ�������������еĲ���ʽ�Ҳ�������ջ��ͬʱ��¼xml�ڵ�ָ�룬���xml��
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




//��д���ķ���txt�ļ��ж�ȡ�ķ�������grammar������
Grammar ReadGrammarFromTXT() {
  FILE *fp;
  fp = fopen("grammar.txt", "r");

  Grammar grammar(1000, 100, 100);
  grammar.vn_amount = 0;
  grammar.vt_amount = 0;
  grammar.production_amount = 0;
  char buffer[1005];
  while (!feof(fp)) {
    fgets(buffer, 1005, fp); //���ж�ȡtxt
    //���������S:������ʼ����
    if (strstr(buffer, "S:") != NULL) {
      printf("S:\n");
      int tmplen = strlen(buffer);
      //��' '��'\n'�滻Ϊ'\0'
      for (int i = 0; i < tmplen; i++) {
        if (buffer[i] == ' ' || buffer[i] == '\n') {
          buffer[i] = '\0';
        }
      }
      grammar.v_start = V(&buffer[3], V::Vtype::Vn);
    //���������Vt:�����ս��
    } else if (strstr(buffer, "Vt:") != NULL) {
      printf("Vt\n");
      int tmplen = strlen(buffer);
      //��' '��'\n'�滻Ϊ'\0'
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
    //���������Vn:�������ս��
    } else if (strstr(buffer, "Vn:") != NULL) {
      printf("Vn\n");
      int tmplen = strlen(buffer);
      //��' '��'\n'�滻Ϊ'\0'
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
    //������ʽ
    } else {
      printf("production\n");
      //��' '��'\n'�滻Ϊ'\0'
      int tmplen = strlen(buffer);
      for (int i = 0; i < tmplen; i++) {
        if (buffer[i] == ' ' || buffer[i] == '\n') {
          buffer[i] = '\0';
        }
      }
      bool if_right_part = false;  // ���Ҫ��ȡ��V+���󲿻����Ҳ�
      grammar.production[grammar.production_amount].left_len = 0;
      grammar.production[grammar.production_amount].right_len = 0;
      //��left��right��Vplus��������ڴ�
      grammar.production[grammar.production_amount].left = new Vplus[100];
      grammar.production[grammar.production_amount].right = new Vplus[100];

      for (int i = 0; i < tmplen - 1; ) {
        //����ǵ�һ��V+������
        if (i == 0) {
          int currentpos = grammar.production[grammar.production_amount].left_len;
          grammar.production[grammar.production_amount].left[currentpos] = Vplus(1);
          grammar.production[grammar.production_amount].left[currentpos].data[0] = V(&buffer[i], ReturnTypeOfV(&buffer[i], grammar));
          grammar.production[grammar.production_amount].left_len++;
          i += strlen(&buffer[i]) + 1;
        //���������->
        } else if (strcmp(&buffer[i], "->") == 0) {
          if_right_part = true;
          i += strlen(&buffer[i]) + 1;
          //�½���һ���Ҳ���Vplus�Ĺ������������
          int currentpos = grammar.production[grammar.production_amount].right_len;
          grammar.production[grammar.production_amount].right[currentpos] = Vplus(100);
          grammar.production[grammar.production_amount].right[currentpos].len = 0;
          
          continue;
        } else {
          //�����������
          if (!if_right_part) {
            int currentpos = grammar.production[grammar.production_amount].left_len;
            grammar.production[grammar.production_amount].left[currentpos] = Vplus(1);  //TODO: �����󲿹̶���ֻ��һ�� V
            grammar.production[grammar.production_amount].left[currentpos].data[0] = V(&buffer[i], ReturnTypeOfV(&buffer[i], grammar));
            grammar.production[grammar.production_amount].left_len++;
            i += strlen(&buffer[i]) + 1;
          //����������Ҳ�
          //������� |
          } else if (strcmp(&buffer[i], "|") != 0) {
            int currentpos = grammar.production[grammar.production_amount].right_len;
            int v_data_pos = grammar.production[grammar.production_amount].right[currentpos].len;
            grammar.production[grammar.production_amount].right[currentpos].data[v_data_pos] = V(&buffer[i], ReturnTypeOfV(&buffer[i], grammar));
            grammar.production[grammar.production_amount].right[currentpos].len++;
            i += strlen(&buffer[i]) + 1;
          //����� |
          } else {
            //�½��Ҳ���Vplus�Ĺ������������
            i += strlen(&buffer[i]) + 1;
            grammar.production[grammar.production_amount].right_len++;  // ���Ҳ���V+����+1
            int currentpos = grammar.production[grammar.production_amount].right_len;
            grammar.production[grammar.production_amount].right[currentpos] = Vplus(100);
            grammar.production[grammar.production_amount].right[currentpos].len = 0;
          }
        }
      }
      grammar.production[grammar.production_amount].right_len++;  // ���Ҳ���V+����+1
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
  //��ȡ�ķ�
  Grammar grammar = ReadGrammarFromTXT();  //txt�ļ����һ��һ��Ҫ�ǿ���
  //����LL1������
  Vplus** ptr_to_LL1_table = CreateLL1_Table(grammar);
  //��ȡ���봮������û����xml���룩
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
  //����LL1�ܿس������xml��ʽ�﷨������
  LL1_Analizer(ptr_to_LL1_table, grammar, input);

  system("pause");
  return 0;
}
//-----------------------------------
//���Գ���Σ�
//int a = 1;
//int b = 2;
//while(true)
//  b = a;
//---------------------------------
