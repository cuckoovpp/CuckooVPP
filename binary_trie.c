#include"binary_trie.h"
#include <memory.h>

static int n_of_nodes=0;
static int ct=1;
static int n_of_lookup=0;
static struct node *root_bg;

struct node * create_node( ) {
    struct node *root=(struct node *) malloc(sizeof(struct node));
    root->left=NULL;
    root->right=NULL;
    root->isleaf=false;
    n_of_nodes++;
    return root;

}

struct node *insert(char *a, struct node *root, int *n) {
    struct node *nd=root;
    int i;
    for(i=0;a[i]!='\0'&& *n>0;i++){
        if(a[i]=='1') {
            if (nd->left == NULL) {
                struct node *left = create_node();
                nd->left = left;
                nd = nd->left;
            }
            else{
                nd = nd->left;
            }
            (*n)--;
        }
        if(a[i]=='0') {
            if (nd->right == NULL) {
                struct node *right = create_node();
                nd->right = right;
                nd = nd->right;
            }
            else{
                nd = nd->right;
            }
            (*n)--;
        }

    }

    return nd;
}

int insert_route_in_trie(__uint64_t *key, int p){
    __uint64_t n=key[0];
    char a[65]="";
    int i=0;
    char rev[65]="";
    char b[65]="";
    int j=0;
    int l,k,r,t;
    char rev1[65]="";
    __uint64_t m=key[1];
    struct node *nd1=NULL;
    struct node *nd=NULL;
    n= ((n>>56)&0xFF)|((n>>40)&0xFF00)|((n>>24)&0xFF0000)|((n>>8)&0xFF000000)|((n<<56)&0xFF00000000000000)|((n<<40)&0x00FF000000000000)|((n<<24)&0x0000FF0000000000)|((n<<8)&0x000000FF00000000);

    m= ((m>>56)&0xFF)|((m>>40)&0xFF00)|((m>>24)&0xFF0000)|((m>>8)&0xFF000000)|((m<<56)&0xFF00000000000000)|((m<<40)&0x00FF000000000000)|((m<<24)&0x0000FF0000000000)|((m<<8)&0x000000FF00000000);


    //root creation

    if(ct==1){
        root_bg=create_node();
        ct=0;
    }

 if (key[0] == 0 && key[1] == 0) {
        
        goto l1;
    } 

else {

  if(n==0){
	memset(rev, 0, 64*sizeof(rev1[0]));
	goto l44;
	}
	
  else{

    while (n!=0){

        r=n%2;
        a[i]=r+48;
        n=n/2;
        i++;
    }

    if(i<64){
        for(k=i;k<64;k++)
            a[k]=48;
    }

}

    for (l = strlen(a)-1,t=0; l >=0 ; --l,t++) {
        rev[t]=a[l];

    }
l44:
  
  nd = insert(rev, root_bg,&p);
}
    if(p>0) {
	
	if(m==0){
		memset(rev1, 0, 64*sizeof(rev1[0]));
	goto l33;
	}

	else{

        while (m!=0){

            k=m%2;
            b[j]=k+48;
            m=m/2;
            j++;
        }

        if(j<64){
            for(k=j;k<64;k++)
                b[k]=48;
        } 


	}

        for (l = strlen(b)-1,t=0; l >=0 ; --l,t++) {
            rev1[t]=b[l];

        }
l33:
        nd1= insert(rev1, nd,&p);
        nd1->isleaf = true;
    }

else {
        
	nd->isleaf=true;
    }

l1:
return 0;

}

struct node * lookup(char *a, struct node *root,int * prefix, int *f_prefix) {
    struct node *nd=root;
    int i;

    for(i=0;a[i]!='\0';i++) {

        if (a[i] == '1') {
            if (nd->left != NULL) {
                (*prefix)++;
                if (nd->left->isleaf){

                    *f_prefix=*prefix;
                }

                nd = nd->left;
		n_of_lookup++;
            }

            else  {
                return nd->left;
            }
        }

        if (a[i] == '0') {
            if (nd->right != NULL) {
                (*prefix)++;
                if (nd->right->isleaf){

                    *f_prefix=*prefix;
                }

                nd = nd->right;
		n_of_lookup++;
            }

            else  {
                return nd->right;
            }
        }

    }


    return nd;

}

int lookup_in_trie(__uint64_t *key){
    __uint64_t n=key[0];
    __uint64_t m=key[1];
    n= ((n>>56)&0xFF)|((n>>40)&0xFF00)|((n>>24)&0xFF0000)|((n>>8)&0xFF000000)|((n<<56)&0xFF00000000000000)|((n<<40)&0x00FF000000000000)|((n<<24)&0x0000FF0000000000)|((n<<8)&0x000000FF00000000);

    m= ((m>>56)&0xFF)|((m>>40)&0xFF00)|((m>>24)&0xFF0000)|((m>>8)&0xFF000000)|((m<<56)&0xFF00000000000000)|((m<<40)&0x00FF000000000000)|((m<<24)&0x0000FF0000000000)|((m<<8)&0x000000FF00000000);
    char a[65]="";
    char rev[65]="";
    int i=0;
    char b[65]="";
    int j=0;
    char rev1[65]="";
    int r,l,k,t;
    int prefix=0,f_prefix=0;
    struct node * r_nd=NULL;

 if (key[0] == 0 && key[1] == 0) {
        
        return 0;
    } 

else {

  if(n==0){
	memset(rev, 0, 64*sizeof(rev1[0]));
	goto l11;
	}
	
else{

    while (n!=0){

        r=n%2;
        a[i]=r+48;
        n=n/2;
        i++;
    }

    if(i<64){
        for(k=i;k<64;k++)
            a[k]=48;
    }

}

    for (l = strlen(a)-1,t=0; l >=0 ; --l,t++) {
        rev[t]=a[l];

    }
l11:
    r_nd=lookup(rev, root_bg,&prefix,&f_prefix);
}

if(m==0)
{
memset(rev1, 0, 64*sizeof(rev1[0]));
goto l22;
}

else {

    while (m!=0){

        k=m%2;
        b[j]=k+48;
        m=m/2;
        j++;
    }

    if(j<64){
        for(k=j;k<64;k++)
            b[k]=48;
    }
}
  
  for (l = strlen(b)-1,t=0; l >=0 ; --l,t++) {
        rev1[t]=b[l];

    }
	l22:
	if(r_nd!=NULL)
        r_nd=lookup(rev1,r_nd,&prefix,&f_prefix);

    return f_prefix;
}

