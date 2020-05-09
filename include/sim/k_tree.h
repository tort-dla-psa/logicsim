#pragma once
#include <list>
#include <memory>
#include <iterator>
#include <cassert>
#include <optional>
#include <queue>

namespace tree_ns{

template<class T>
struct node {
    T value;

    node *parent = nullptr,
        *neighbour_next = nullptr, *neighbour_prev = nullptr,
        *children_beg = nullptr, *children_end = nullptr;

    node()=default;

    node(T &&rhs)
        requires std::is_move_constructible_v<T>
        :value(std::move(rhs))
    {}

    node(const T &rhs)
        requires std::is_copy_constructible_v<T>
        :value(rhs)
    {}
};

template <class T, class node_allocator = std::allocator<node<T>>>
class k_tree {
    using node_ = node<T>;
    node_allocator m_alloc_;
    node_* m_root, *m_foot;
public:
    typedef T* pointer;
    typedef T  value_type;
    typedef T& reference;

    class iterator_base {
    protected:
        friend class k_tree;
        node_ *n;

        template<class Type>
        Type& do_decrement_ref(Type*& ptr, size_t num){
            while(num > 0) {
                --(ptr);
                --num;
            }
            return (*ptr);
        }

        template<class Type>
        Type do_decrement(Type* ptr){
            auto copy = *ptr;
            --(*ptr);
            return copy;
        }

        template<class Type>
        Type& do_increment_ref(Type*& ptr, size_t num){
            while(num > 0) {
                ++(*ptr);
                --num;
            }
            return (*ptr);
        }

        template<class Type>
        Type do_increment(Type* ptr){
            auto copy = *ptr;
            ++(*ptr);
            return copy;
        }

        template<class Type>
        void get_children_beg(Type*& ptr){
            while(ptr->children_beg){
                ptr = ptr->children_beg;
            }
        }
        template<class Type>
        void get_children_end(Type*& ptr){
            while(ptr->children_end){
                ptr = ptr->children_end;
            }
        }
    public:
        typedef T* pointer;
        typedef T  value_type;
        typedef T& reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        iterator_base(node_ *n = nullptr)
            :n(n){}

        T& operator*() const{
            return n->value;
        }

        T* operator->() const{
            return &(n->value);
        }
        
        auto begin_children_iterator() const {
            if(!n->children_beg) {
                return end_children_iterator();
            }

            auto result = neighbour_iterator(n->children_beg); //autosets parent
            return result;
        }

        auto end_children_iterator() const {
            neighbour_iterator result;
            result.parent_ = this->n;
            return result;
        }

        size_t size()const{
            if(!n->children_beg)
                return 0;
            return std::distance(std::begin(n->children_beg), std::end(n->children_end));
        }

        bool  operator == (const iterator_base &rhs) const{
            return (rhs.n == this->n);
        }
        bool  operator != (const iterator_base &rhs) const{
            return (rhs.n != this->n);
        }
    };

    class depth_first_node_first_iterator : public iterator_base {
        bool m_skip_children = false;
    public:
        depth_first_node_first_iterator(node_* n = nullptr)
            :iterator_base(n){}

        void skip_children(bool flag=true){
            this->m_skip_children = flag;
        }

        auto& operator--(){
            assert(this->n != 0);
            if(this->n->neighbour_prev) {
                this->n = this->n->neighbour_prev;
                if(!this->m_skip_children){
                    iterator_base::get_children_end(this->n);
                }
            } else {
                this->n = this->n->parent;
            }
            this->m_skip_children = false;
            return *this;
        }

        auto& operator++(){
            assert(this->n);
            if(!this->m_skip_children && this->n->children_beg){
                this->n = this->n->children_beg;
                return *this;
            }
            this->m_skip_children = false;
            while(!this->n->neighbour_next) {
                this->n = this->n->parent;
                if(!this->n)
                    return *this;
            }
            this->n = this->n->neighbour_next;
            return *this;
        }

        auto  operator++(int){ return iterator_base::do_increment(this); }
        auto  operator--(int){ return iterator_base::do_decrement(this); }
        auto& operator+=(size_t num){ return do_increment_ref(this, num); }
        auto& operator-=(size_t num){ return do_decrement_ref(this, num); }
    };

/*
    class depth_firt_children_first_iterator : public iterator_base {
    public:
        depth_firt_children_first_iterator(node_ *n):
            iterator_base(n){}

        auto& operator++(){
            assert(this->n);
            if(!this->n->neighbour_next) {
                this->n= this->n->parent;
            } else {
                this->node_ = this->n->neighbour_next;
                children_begin(this->n);
            }
            return *this;
        }
        auto& operator--(){
            assert(this->n);
            if(!this->n->children_end){
                while(!this->n->neighbour_prev){	
                    this->n = this->n->parent;
                }
                this->n = this->n->neighbour_prev;
            } else {
                this->n = this->n->children_end;
            }
            return *this;
        }
        auto  operator++(int){ return do_increment(this); }
        auto  operator--(int){ return do_decrement(this); }
        auto& operator+=(size_t num){ return do_increment_ref(this, num); }
        auto& operator-=(size_t num){ return do_decrement_ref(this, num); }
    };

    class width_first_queue_iterator : public iterator_base {
    public:
        width_first_queue_iterator(node_ *n = nullptr)
            :iterator_base(n)
        {
            if(n){
                travers_q.emplace(n);
            }
        }

        auto& operator++(){
            assert(this->n);

            // Add child nodes and pop current node_
            auto sib = this->begin_children_iterator();
            auto end_children_it = this->end_children_iterator();
            while(sib != end_children_it){
                travers_q.emplace(sib.n);
                ++sib;
            }
            travers_q.pop();
            if(travers_q.size() > 0){
                this->n = travers_q.front();
            }else{
                this->n = nullptr;
            }
            return (*this);
        }
        auto  operator++(int){ return do_increment(this); }
        auto& operator+=(const size_t &num){ return do_increment_ref(this, num); }
    private:
        std::queue<node_*> travers_q;
    };
    */

    class neighbour_iterator : public iterator_base {
    public:
        neighbour_iterator(node_ *n = nullptr)
            :iterator_base(n)
        {
            if(!this->n) return;
            if(this->n->parent)
                parent_ = this->n->parent;
        }

        auto& operator++(){
            if(this->n)
                this->n = this->n->neighbour_next;
            return *this;
        }
        auto& operator--(){
            if(this->n){
                this->n = this->n->neighbour_prev;
            } else {
                assert(parent_);
                this->n = parent_->children_end;
            }
            return *this;
        }
        auto  operator++(int){ return do_increment(this); }
        auto  operator--(int){ return do_decrement(this); }
        auto& operator+=(size_t num){ return do_increment_ref(this, num); }
        auto& operator-=(size_t num){ return do_decrement_ref(this, num); }
    private:
    friend class iterator_base;
    friend class k_tree;
        node_* parent_;
    };

/*
    class leaf_iterator : public iterator_base {
    public:
        leaf_iterator(node_ *n = nullptr, node_ *top = nullptr)
            :iterator_base(n), top(top)
        {}

        bool  operator==(const leaf_iterator &rhs) const{
            return (rhs.n == this->n && rhs.top == this->top);
        }
        bool  operator!=(const leaf_iterator &rhs) const{
            return !(rhs == *this);
        }
        auto& operator++(){
            assert(this->n != 0);
            if(this->n->children_beg) {
                iterator_base::get_children_beg(this->n);
            } else {
                while(!this->n->neighbour_next) { 
                    if (!this->n->parent){
                        return *this;
                    }
                    this->n = this->n->parent;
                    if (top && this->n == top){
                        return *this;
                    }
                }
                this->n = this->n->neighbour_next;
                iterator_base::get_children_beg(this->n);
            }
            return *this;
            
        }
        auto& operator--(){
            assert(this->n != 0);
            while (!this->n->neighbour_prev) {
                if (!this->n->parent){
                    return *this;
                }
                this->n = this->n->parent;
                if (this->n == top){
                    return *this; 
                }
            }
            this->n = this->n->neighbour_prev;
            iterator_base::get_children_end(this->n);
            return *this;
        }
        auto  operator++(int){ do_increment(this); }
        auto  operator--(int){ do_decrement(this); }
        auto& operator+=(const size_t &num){ do_increment_ref(this, num); }
        auto& operator-=(const size_t &num){ do_decrement_ref(this, num); }
    private:
        node_* top;
    };
    */

    k_tree() {
        p_init();
    }

    k_tree(const T &val)
        requires std::is_copy_constructible_v<T>
    {
        p_init();
        set_root(val);
    }

    k_tree(T&& val)
        requires std::is_move_constructible_v<T>
    {
        p_init();
        set_root(std::move(val));
    }

    k_tree(const k_tree<T, node_allocator> &rhs)
        requires std::is_copy_constructible_v<T>
    {
        p_init();
        p_copy(rhs);
    }

    k_tree(k_tree<T, node_allocator>&& rhs)
        requires std::is_move_constructible_v<T>
    {
        p_init();
        p_move(std::move(rhs));
    }

    ~k_tree(){
        clear();
        m_alloc_.deallocate(m_root,1);
        m_alloc_.deallocate(m_foot,1);
    }

    inline k_tree<T, node_allocator>& operator=(const k_tree<T, node_allocator> &rhs)
        requires std::is_copy_constructible_v<T>
    {
        if(&rhs != this){
            p_copy(rhs);
        }
        return *this;
    }

	inline k_tree<T, node_allocator>& operator=(k_tree<T, node_allocator>&& rhs)
        requires std::is_move_constructible_v<T>
    {
        if(&rhs == this){
            return *this;
        }
		clear();

		m_root->neighbour_next = rhs.m_root->neighbour_next;
		m_root->neighbour_next->neighbour_prev = m_root;
		m_foot->neighbour_prev = rhs.m_root->neighbour_next;
		m_foot->neighbour_prev->neighbour_next = m_foot;

		rhs.m_root->neighbour_next = rhs.m_foot;
		rhs.m_foot->neighbour_prev = rhs.m_root;
        return *this;
    }

    inline auto root() const {
        return depth_first_node_first_iterator(m_root->neighbour_next);
    }
    
    inline auto begin() const{
        return depth_first_node_first_begin();
    }
    inline auto end() const{
        return depth_first_node_first_end();
    }
    inline auto depth_first_node_first_begin() const{
        return depth_first_node_first_iterator(m_root->neighbour_next);
    }
    inline auto depth_first_node_first_end() const{
        return depth_first_node_first_iterator(m_foot);
    }
    inline auto depth_first_node_first_begin(const iterator_base &it) const{
        return depth_first_node_first_iterator(it.n);
    }
    inline auto depth_first_node_first_end(const iterator_base &it) const{
        return depth_first_node_first_iterator(it.n->neighbour_next);
    }
    inline auto depth_children_begren_first_begin() const {
        auto bak = m_root->neighbour_next;
        while(bak->children_beg && bak!= m_foot){
            bak = bak->children_beg;
        }
        return depth_children_begren_first_iterator(bak);
    }
    inline auto depth_children_begren_first_end() const{
        return depth_children_begren_first_iterator(m_foot);
    }
    /*
    inline auto width_first_begin() const{
        return width_first_queue_iterator(m_root->neighbour_next);
    }
    inline auto width_first_end() const{
        return width_first_queue_iterator();
    }
    inline auto width_first_begin(const iterator_base &it) const{
        return width_first_queue_iterator(it.n);
    }
    inline auto width_first_end(const iterator_base &it) const{
        return width_first_queue_iterator();
    }
    */
    inline auto children_begin(const iterator_base &it) const{
        assert(it.n);
        if(!it.n->children_beg) {
            return children_end(it);
        }
        return neighbour_iterator(it.n->children_beg);
    }
    inline auto children_end(const iterator_base &it) const{
        neighbour_iterator result;//TODO: two params constructor
        result.parent_ = it.n;
        return result;
    }
    /*
    inline auto leaf_begin(const iterator_base &it) const{
        auto bak = it.n;
        while(bak->children_beg){
            bak = bak->children_beg;
        }
        return leaf_iterator(bak, it.n);
    }
    inline auto leaf_end(const iterator_base &it) const{
        return leaf_iterator(it.n, it.n);
    }
    */
    inline auto get_parent(const iterator_base &it) const{
        assert(it.n);
        return depth_first_node_first_iterator(it.n->parent);
    }
    inline auto neighbour_prev(const iterator_base &it) const{
        assert(it.n);
        depth_first_node_first_iterator result(it);
        result.n = it.n->neighbour_prev;
        return result;
    }
    inline auto neighbour_next(const iterator_base &it) const{
        assert(it.n);
        depth_first_node_first_iterator result(it);
        result.n = it.n->neighbour_next;
        return result;
    }

    inline void clear() {
        if(!m_root){
            return;
        }
        while(m_root->neighbour_next != m_foot) {
            auto it = depth_first_node_first_iterator(m_root->neighbour_next);
            erase(it);
        }
    }

    template<class It>
    It erase(It it) {
        node_* cur = it.n;
        assert(cur != m_root);
        auto ret = it;
        ++ret;
        erase_children(it);

        auto& nd_retie1 = (!cur->neighbour_prev)? cur->parent->children_beg:
            cur->neighbour_prev->neighbour_next;
        nd_retie1 = cur->neighbour_next;

        auto& nd_retie2 = (!cur->neighbour_next)? cur->parent->children_end:
            cur->neighbour_next->neighbour_prev; 
        nd_retie2 = cur->neighbour_prev;

        m_alloc_.deallocate(cur, 1);
        return ret;
    }

    inline void erase_children(const iterator_base &it) {
        if(!it.n){
            return;
        }

        node_* cur = it.n->children_beg,
            *prev;

        while(cur) {
            prev = cur;
            cur = cur->neighbour_next;
            auto it = depth_first_node_first_iterator(prev); //shadows "it"
            erase_children(it);
            m_alloc_.deallocate(prev, 1);
        }
        it.n->children_beg = it.n->children_end = nullptr;
    }

    inline auto set_root(T& x){
        return set_root(std::move(x));
    }
    inline auto set_root(T&& x){
        assert(m_root->neighbour_next == m_foot);
        auto it = depth_first_node_first_iterator(m_foot);
        return insert_before(it, std::move(x));
    }

    template<class It, class Arg>
    inline auto child_append(It it, Arg&& x){
        // If your program fails here you probably used 'child_append' to add the top
        // node_ to an empty k_tree. From version 1.45 the top element should be added
        // using 'insert_before'. See the documentation for further information, and sorry about
        // the API change.
        assert(it.n != m_root);
        assert(it.n);

        auto tmp = m_alloc_.allocate(1);
        m_alloc_.construct(tmp, std::forward<Arg>(x));
        tmp->parent = it.n;

        auto& nd_tie = (it.n->children_end)? it.n->children_end->neighbour_next:
            it.n->children_beg;
        nd_tie =tmp;

        tmp->neighbour_prev = it.n->children_end;
        it.n->children_end = tmp;
        return depth_first_node_first_iterator(tmp);
    }

    template<class It, class Arg>
    inline auto child_prepend(It it, Arg&& x) {
        assert(it.n != m_root);
        assert(it.n);

        auto tmp = m_alloc_.allocate(1);
        m_alloc_.construct(tmp, std::forward<Arg>(x));
        tmp->parent = it.n;

        auto& nd_tie = (it.n->children_beg)? it.n->children_beg->neighbour_prev:
            it.n->children_end;
        nd_tie =tmp;

        tmp->neighbour_next = it.n->children_beg;
        it.n->children_beg = tmp;
        return depth_first_node_first_iterator(tmp);
    }

    template<class It, class Arg>
    inline auto insert_before(It it, Arg&& x){
        if(!it.n) {
            it.n = m_foot;
        }
        assert(it.n != m_root);

        auto tmp = m_alloc_.allocate(1);
        m_alloc_.construct(tmp, std::forward<Arg>(x));
        tmp->parent = it.n->parent;
        tmp->neighbour_next = it.n;
        tmp->neighbour_prev = it.n->neighbour_prev;
        it.n->neighbour_prev = tmp;

        if(!tmp->neighbour_prev) {
            if(tmp->parent) // when inserting nodes at the m_root, there is no parent
                tmp->parent->children_beg = tmp;
        } else {
            tmp->neighbour_prev->neighbour_next = tmp;
        }
        return It(tmp);
    }

    template<class It, class Arg>
    inline auto insert_after(It it, Arg&& x) {
        if(!it.n) {
            it.n = m_foot;
        }
        auto tmp = m_alloc_.allocate(1);
        m_alloc_.construct(tmp, std::forward<Arg>(x));

        tmp->parent = it.n->parent;
        tmp->neighbour_prev = it.n;
        tmp->neighbour_next = it.n->neighbour_next;
        it.n->neighbour_next = tmp;

        if(!tmp->neighbour_next) {
            if(tmp->parent) // when inserting nodes at the m_root, there is no parent
                tmp->parent->children_end = tmp;
        } else {
            tmp->neighbour_next->neighbour_prev = tmp;
        }
        return It(tmp);
    }

    template<class It>
    inline auto replace(It dst, It src) {
        assert(dst.n != m_root);
        auto src_cur = src.n;
        auto src_start = src.n;
        auto dst_cur = dst.n;

        // replace the node at dst with m_root of the replacement tree at from
        erase_children(dst);	
        auto tmp = m_alloc_.allocate(1);
        m_alloc_.construct(tmp);
        std::swap(tmp->value, src.n->value);
        if(!dst_cur->neighbour_prev) {
            if(dst_cur->parent){
                dst_cur->parent->children_beg=tmp;
            }
        } else {
            dst_cur->neighbour_prev->neighbour_next=tmp;
        }
        tmp->neighbour_prev=dst_cur->neighbour_prev;
        if(!dst_cur->neighbour_next) {
            if(dst_cur->parent){
                dst_cur->parent->children_end=tmp;
            }
        } else {
            dst_cur->neighbour_next->neighbour_prev=tmp;
        }
        tmp->neighbour_next=dst_cur->neighbour_next;
        tmp->parent=dst_cur->parent;
        m_alloc_.destroy(dst_cur);
        m_alloc_.deallocate(dst_cur,1);
        dst_cur=tmp;
        
        // only at this stage can we fix 'last'
        auto last=src.n->neighbour_next;

        depth_first_node_first_iterator toit=tmp;
        // copy all children
        do {
            assert(src_cur!=0);
            if(src_cur->children_beg != 0) {
                src_cur=src_cur->children_beg;
                toit=child_append(toit, src_cur->value);
            } else {
                while(src_cur->neighbour_next==0 && src_cur!=src_start) {
                    src_cur=src_cur->parent;
                    toit=get_parent(toit);
                    assert(src_cur);
                }
                src_cur=src_cur->neighbour_next;
                if(src_cur!=last) {
                    toit=child_append(get_parent(toit), src_cur->value);
                }
            }
        } while(src_cur!=last);

        return dst_cur;
    }

    template<class Arg>
    inline auto set_value(iterator_base &it, Arg&& x){
        std::swap(it.n->value, x);
        return it;
    }

    template<class Arg>
    inline auto root_prepend(Arg&& x) {
        if(m_root->neighbour_next == m_foot) {
            return this->set_root(std::forward<Arg>(x));
        }
        auto bak = m_root->neighbour_next;
        {
            auto it = depth_first_node_first_iterator(m_foot);
            insert_before(it, std::forward<Arg>(x));
        }

        auto new_head_node = m_root->neighbour_next->neighbour_next;
        m_root->neighbour_next = new_head_node;
        new_head_node->children_beg = bak;
        new_head_node->children_end = bak;
        new_head_node->neighbour_next = m_foot;
        new_head_node->neighbour_prev = m_root;

        m_foot->neighbour_prev = new_head_node;

        bak->neighbour_next = bak->neighbour_prev = nullptr;
        bak->parent = new_head_node;

        return depth_first_node_first_iterator(m_root());
    }

    inline auto move_after(const iterator_base &source, const iterator_base &destination){
        auto dst = destination.n;
        auto src = source.n;
        assert(dst && src);

        if(dst == src ||
            (dst->neighbour_prev && dst->neighbour_prev == src))
        {
            return this->depth_first_node_first_begin(source);
        }

        if(src->neighbour_prev){ // take src out of the k_tree
            src->neighbour_prev->neighbour_next = src->neighbour_next;
        }else{
            src->parent->children_beg = src->neighbour_next;
        }
        if(src->neighbour_next){
            src->neighbour_next->neighbour_prev = src->neighbour_prev;
        }else{
            src->parent->children_end = src->neighbour_prev;
        }

        // connect src to the new point
        auto &nd_retie = (dst->neighbour_prev)?dst->neighbour_prev->neighbour_next:
            dst->parent->children_beg;
        nd_retie = src;

        src->neighbour_prev = dst->neighbour_prev;
        dst->neighbour_prev = src;
        src->neighbour_next = dst;
        src->parent = dst->parent;
        return depth_first_node_first_iterator(src);
    }
    inline auto move_before(const iterator_base &source, const iterator_base &destination);

    inline size_t size()const{
        depth_first_node_first_iterator it(m_root);
        return size(it);
    }

    inline size_t size(const iterator_base &it) const{
        size_t i=0;
        depth_first_node_first_iterator it_ = it, eit_ = it;
        ++eit_;
        while(it_ != eit_) {
            ++i;
            ++it_;
        }
        return i;
    }

    inline bool empty() const {
        depth_first_node_first_iterator it, eit;
        return it == eit; //TODO:check
    }

    inline size_t max_depth()const{
        size_t ret;
        for(auto it = m_root->neighbour_next; it != m_foot; it = it->neighbour_next){
            ret = std::max(ret, max_depth(it));
        }
        return ret;
    }
    inline size_t max_depth(const iterator_base &it) const{
        auto tmp = it.n;

        if(tmp || tmp == m_root || tmp == m_foot)
            return 0;

        long long curdepth = 0, maxdepth = 0;
        while(true) { // try to walk the bottom of the k_tree
            while(!tmp->children_beg) {	
                if(tmp == it.n){
                    return maxdepth;
                }
                if(!tmp->neighbour_next) {
                    // try to walk up and then right again
                    do {
                        tmp = tmp->parent;
                        if(!tmp){
                            return maxdepth;
                        }
                        --curdepth;
                    } while(!tmp->neighbour_next);
                }

                if(tmp == it.n){
                    return maxdepth;
                }
                tmp = tmp->neighbour_next;
            }

            tmp = tmp->children_beg;
            ++curdepth;
            maxdepth = std::max(curdepth, maxdepth);
        } 
        return -1; //NOTE:should not reach
    }
    inline bool valid(const iterator_base &it) const{
        return !(it.n==0 || it.n==m_foot || it.n==m_root);
    }
private:
    inline void p_init(){ 
        m_root = m_alloc_.allocate(1);
        m_foot = m_alloc_.allocate(1);
        m_root->neighbour_prev = m_root->children_beg = m_root->children_end = m_root->parent = nullptr;
        m_foot->neighbour_next = m_foot->children_beg = m_foot->children_end = m_foot->parent = nullptr;
        m_foot->neighbour_prev = m_root;
        m_root->neighbour_next = m_foot;
    }

    void p_help_construct_children(depth_first_node_first_iterator &dst,
        const k_tree<T, node_allocator> &rhs)
    {
        p_help_construct_children(dst, rhs, rhs.begin());
    }
    void p_help_construct_children(const depth_first_node_first_iterator &dst,
        const k_tree<T, node_allocator> &rhs,
        k_tree<T, node_allocator>::depth_first_node_first_iterator rhs_src)
    {
        auto child_it = rhs.children_begin(rhs_src);
        auto end = rhs.children_end(rhs.m_root);
        for(child_it; child_it != end; ++child_it) {
            auto new_src = rhs.depth_first_node_first_begin(child_it);
            auto new_dst = child_append(dst, (*child_it));
            p_help_construct_children(new_dst, rhs, new_src);
        }
    }

    void p_copy(const k_tree<T, node_allocator> &rhs){
        clear();
        auto rt = rhs.m_root;
        set_root(rt->value);

        auto it = begin();
        for(auto rhs_it = rhs.begin(); rhs_it != rhs.end(); ++rhs_it, ++it){
            it.n->parent = rhs_it.n->parent;
            it.n->neighbour_next = rhs_it.n->neighbour_next;
            it.n->neighbour_prev = rhs_it.n->neighbour_prev;
            it.n->children_beg = rhs_it.n->children_beg;
            it.n->children_end = rhs_it.n->children_end;
            it.n->value = rhs_it.n->value;
        }
    }

    void p_move(k_tree<T, node_allocator> &&rhs){
        clear();
        auto rt = rhs.m_root;
        set_root(std::move(rt->value));

        auto it = begin();
        m_alloc_.deallocate(it.n, 1);
        for(auto rhs_it = rhs.begin(); rhs_it != rhs.end(); ++rhs_it){
            it = insert_after(it, std::move(rhs_it.n->value));
            auto ch_it = rhs_it.begin_children_iterator();
            auto ch_end = rhs_it.end_children_iterator();
            for(; ch_it != ch_end; ++ch_it){
                child_append(it, std::move(ch_it.n->value));
            }
        }
    }
};

//NOTE: distance can be computed only in depth 
template<class T>
inline auto distance(const typename k_tree<T>::iterator_base &it1, const typename k_tree<T>::iterator_base &it2){
    auto it = it1.n;
	assert(it);
	size_t result = 0;
	while(it->parent && it != it2.n) {
		it = it->parent;
		++result;
	}
	return result;
}

};