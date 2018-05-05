/* Hashtable lib */
typedef struct node_t_ {
    int key;
    int val;
    struct node_t_ *next;
} node_t;

typedef struct hashtable_t_ {
    node_t **table;
    int elem_count;
    int curr_size_idx;
} hashtable_t;

#define DEFAULT_SIZE    17
unsigned int primes[] =
                {17, 37, 79, 163, 331, 673, 1361, 2729, 5471, 10949, 21911,
                 43853, 87719, 175447, 350899, 701819, 1403641, 2807303,
                 5614657, 11229331, 22458671, 44917381, 89834777, 179669557,
                 359339171, 718678369, 1437356741, 2874713497};


void hashtable_init (hashtable_t *ht)
{
    ht->elem_count = 0;
    ht->curr_size_idx = 0;
    ht->table = calloc(DEFAULT_SIZE, sizeof(node_t *));
}

void hashtable_destroy (hashtable_t *ht)
{
    int i;
    node_t *head, *tmp;
    
    for (i = 0; i < primes[ht->curr_size_idx]; i++) {
        head = ht->table[i];
        while (head) {
            tmp = head;
            head = head->next;
            free(tmp);
        }
    }
    if (ht->table) free(ht->table);
    ht->elem_count = 0;
    ht->curr_size_idx = 0;
}

static unsigned int compute_hash (hashtable_t *ht, int key)
{
    if (key < 0) key = key & 0x7FFFFFFF;
    
    return (key % primes[ht->curr_size_idx]);
}

bool hashtable_lookup (hashtable_t *ht, int key, int *val)
{
    unsigned int hash;
    node_t *head;
    bool ret = false;
    
    hash = compute_hash(ht, key);
    head = ht->table[hash];
    while (head) {
        if (head->key == key) {
            ret = true;
            *val = head->val;
            break;
        }
        head = head->next;
    }
    
    return (ret);
}

static void hashtable_insert_internal (hashtable_t *ht, node_t *new)
{
    unsigned int hash;
    node_t *head;
    
    hash = compute_hash(ht, new->key);
    new->next = ht->table[hash];
    ht->table[hash] = new;
    ht->elem_count++;
}

static void hashtable_resize (hashtable_t *ht, bool incr)
{
    int i, old_size = primes[ht->curr_size_idx];
    node_t **old_table = ht->table;
    node_t *head;
    
    ht->elem_count = 0;
    if (incr) {
        ht->curr_size_idx++;
    } else {
        ht->curr_size_idx--;
    }
    ht->table = calloc(primes[ht->curr_size_idx], sizeof(node_t *));
    
    for (i = 0; i < old_size; i++) {
        head = old_table[i];
        while (head) {
            hashtable_insert_internal(ht, head);
            head = head->next;
        }
    }
    free(old_table);
}

bool hashtable_insert (hashtable_t *ht, int key, int val)
{
    node_t *new = calloc(1, sizeof(node_t));
    new->key = key;
    new->val = val;
    
    if (ht->elem_count == primes[ht->curr_size_idx]) {
        hashtable_resize(ht, true);
    }
    
    hashtable_insert_internal(ht, new);
    
    return (true);
}

bool hashtable_remove (hashtable_t *ht, int key, int *val)
{
    bool ret = false;
    unsigned int hash;
    node_t *head, *tmp = NULL;
    
    hash = compute_hash(ht, key);
    head = ht->table[hash];
    while (head) {
        if (head->key == key) {
            ret = true;
            ht->elem_count--;
            *val = head->val;
            if (tmp) {
                tmp->next = head->next;
                free(head);
            } else {
                ht->table[hash] = head->next;
            }
        }
        tmp = head;
        head = head->next;
    }
    
    if ( ht->curr_size_idx && ((ht->elem_count * 4) <= primes[ht->curr_size_idx]) ) {
        hashtable_resize(ht, false);
    }
    
    return (ret);
}
/* Hashtable lib */

