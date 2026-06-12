#ifndef _NZD_STRING_HPP_
#define _NZD_STRING_HPP_ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>

class string {
public:
    constexpr static size_t sso_capacity = 23;

    string() = default;
    explicit string(const char* s) { assign(s, strlen(s) + 1); }
    string(size_t n_ch, char ch = '\0') { assign(n_ch + 1, ch); }

    string(const string& other) {
        if (other.is_on_heap()) heap_assign(other.data(), other.size());
        else memcpy(&storage, &other.storage, sizeof(storage));
    }
    string& operator=(const string& other) {
        if (this != &other) assign(other.data(), other.size());
        return *this;
    }

    string(string&& other) noexcept {
        memcpy(&storage, &other.storage, sizeof(storage));
        other = "";
    }
    string& operator=(string&& other) noexcept {
        if (this != &other) {
            if (is_on_heap()) {
                free_heap_ptr_sized_dirty();
            }
            memcpy(&storage, &other.storage, sizeof(storage));
            other = "";
        }
        return *this;
    }

    string& operator=(const char* s) {
        assign(s, strlen(s) + 1);
        return *this;
    }

    const char* data() const {
        if (is_on_heap()) return storage.heap.ptr_const();
        else return storage.sso.buf_const();
    }

    char* data_mutable() {
        if (is_on_heap()) return get_heap_ptr();
        else return get_sso_buf();
    }

    size_t capacity() const {
        if (is_on_heap()) return get_heap_capacity();
        else return get_sso_capacity();
    }

    size_t size() const {
        if (is_on_heap()) return get_heap_size();
        else return get_sso_size();
    }

    size_t count() const {
        if (is_on_heap()) return get_heap_size() == 0 ? 0 : get_heap_size() - 1;
        else return get_sso_size() == 0 ? 0 : get_sso_size() - 1;
    }

    void assign(const char* s, size_t n_byte) {
        if (n_byte > get_sso_capacity()) heap_assign(s, n_byte);
        else sso_assign(s, n_byte);
    }

    void assign(size_t n_byte, char ch) {
        if (n_byte > get_sso_capacity()) heap_assign(n_byte, ch);
        else sso_assign(n_byte, ch);
    }

    void reserve(size_t n_byte) {
        // if enough, do nothing
        // so if using OSS and n_byte <= oss_capacity, never do anything
        if (n_byte <= capacity()) return;
        else realloc_buffer(n_byte);
    }

    bool is_on_heap() const { return get_mode_bit() == 1; }

    ~string() {
        if (is_on_heap()) {
            free_heap_ptr_sized_dirty();
        }
    }

    const unsigned char* get_self_bytes() const { return storage.bytes; }

private:
    // return capacity after reallocate
    void realloc_buffer(size_t n_byte_capa) {
        assert(n_byte_capa > 0);
        if (!is_on_heap()) {
            // if current is on SSO
            // if request n_byte_capa > sso_capacity
            if (n_byte_capa > get_sso_capacity()) {
                // need transfer data to heap
                size_t old_sso_size = get_sso_size();

                size_t new_heap_capa = to_aligned_size(n_byte_capa);
                char* new_buf_ptr = (char*)malloc(new_heap_capa);
                memcpy(new_buf_ptr, get_sso_buf_const(), old_sso_size);

                // start writing storage, original data on SSO will be broken
                set_heap_size(old_sso_size);
                set_heap_capacity(new_heap_capa);
                set_heap_ptr(new_buf_ptr);
                set_heap_mode();
            }
            // if n_byte_capa <= sso_capacity, we have no way to decrease capacity

        } else {
            // if current is on heap
            // increase heap capacity when n_byte_capa > heap_capacity
            // decrease heap capacity when n_byte_capa > heap_size && n_byte_capa < heap_capacity
            // if n_byte_capa < heap_size, decrease heap capacity and remove extra data
            if (n_byte_capa != get_heap_capacity()) {
                size_t old_heap_capa = get_heap_capacity();
                size_t old_heap_size = get_heap_size();
                char* old_heap_ptr = get_heap_ptr();

                size_t new_heap_capa = to_aligned_size(n_byte_capa);
                char* new_buf_ptr = (char*)malloc(new_heap_capa);

                size_t copy_size = old_heap_size < new_heap_capa ? old_heap_size : new_heap_capa;
                memcpy(new_buf_ptr, old_heap_ptr, copy_size);

                free_sized(old_heap_ptr, old_heap_capa);
                set_heap_ptr(new_buf_ptr);
                set_heap_capacity(new_heap_capa);
                size_t final_size = n_byte_capa < old_heap_size ? n_byte_capa : old_heap_size;
                set_null(final_size);
                set_heap_size(final_size);
            }
            // if n_byte_capa = heap_size, do nothing
        }
    }

    void sso_assign(const char* s, size_t n_byte) {
        assert(n_byte <= get_sso_capacity());
        assert(n_byte > 0 && s[n_byte - 1] == '\0');
        if (is_on_heap()) free_heap_ptr_sized_dirty();
        set_sso_size(n_byte);
        set_sso_buf(s, n_byte);
        set_sso_mode();
    }

    void sso_assign(size_t n_byte, char ch) {
        assert(n_byte <= get_sso_capacity());
        if (is_on_heap()) free_heap_ptr_sized_dirty();
        set_sso_size(n_byte);
        set_sso_buf(n_byte, ch);
        set_sso_mode();
    }

    void heap_assign(const char* s, size_t n_byte) {
        assert(n_byte > 0 && s[n_byte - 1] == '\0');
        // if current data on heap, free first
        if (is_on_heap()) free_heap_ptr_sized_dirty();
        size_t heap_capa = to_aligned_size(n_byte);
        char* heap_ptr = (char*)malloc(heap_capa);
        memcpy(heap_ptr, s, n_byte);
        set_heap_capacity(heap_capa);
        set_heap_ptr(heap_ptr);
        set_heap_size(n_byte);
        set_heap_mode();
    }

    void heap_assign(size_t n_byte, char ch) {
        // if current data on heap, free first
        if (is_on_heap()) free_heap_ptr_sized_dirty();
        size_t heap_capa = to_aligned_size(n_byte);
        char* heap_ptr = (char*)malloc(heap_capa);
        memset(heap_ptr, ch, n_byte);
        heap_ptr[n_byte] = '\0';
        set_heap_capacity(heap_capa);
        set_heap_ptr(heap_ptr);
        set_heap_size(n_byte);
        set_heap_mode();
    }

    void set_null(size_t n_byte) {
        assert(n_byte <= capacity());
        data_mutable()[n_byte] = '\0';
    }

private:
    unsigned char get_mode_bit() const { return storage.mode; }

    size_t get_sso_size() const { return storage.sso.size(); }
    size_t get_sso_capacity() const { return storage.sso.capacity(); }
    char* get_sso_buf() { return storage.sso.buf(); }
    const char* get_sso_buf_const() const { return storage.sso.buf_const(); }
    void set_sso_size(size_t n_byte) { storage.sso.set_size(n_byte); }
    void set_sso_buf(const char* s, size_t n_byte) { storage.sso.set_buf(s, n_byte); }
    void set_sso_buf(size_t n_byte, char ch) { storage.sso.set_buf(n_byte, ch); }
    void set_sso_mode() { storage.mode = 0; }

    size_t get_heap_size() const { return storage.heap.size(); }
    size_t get_heap_capacity() const { return storage.heap.capacity(); }
    char* get_heap_ptr() { return storage.heap.ptr(); }
    const char* get_heap_ptr_const() const { return storage.heap.ptr_const(); }
    void set_heap_size(size_t n_byte) { storage.heap.set_size(n_byte); }
    void set_heap_capacity(size_t n_byte) { storage.heap.set_capacity(n_byte); }
    void set_heap_ptr(char* ptr) { storage.heap.set_ptr(ptr); }
    void set_heap_mode() { storage.mode = 1; }

    void free_heap_ptr_sized_dirty() {
        assert(is_on_heap());
        free_sized(storage.heap.ptr(), storage.heap.capacity());
    }

    constexpr static size_t to_aligned_size(size_t n_byte) {
        return (n_byte & 1) == 0 ? n_byte : n_byte + 1;
    }

public:
    class sso_storage {
        unsigned char size_;
        char buf_[string::sso_capacity];

    public:
        size_t size() const { return size_; }
        size_t capacity() const { return sizeof(buf_); }
        char* buf() { return buf_; }
        const char* buf_const() const { return buf_; }

        void set_size(size_t n) {
            assert(n <= sizeof(buf_));
            size_ &= 1;
            size_ |= n << 1;
        }
        void set_buf(const char* s, size_t n_byte) {
            assert(n_byte <= sizeof(buf_));
            memcpy(buf_, s, n_byte);
        }
        void set_buf(size_t n_byte, char ch) {
            assert(n_byte <= sizeof(buf_));
            memset(buf_, ch, n_byte);
            buf_[n_byte] = '\0';
        }
    };

    class heap_storage {
        size_t capacity_;
        size_t size_;
        char* ptr_;

    public:
        size_t capacity() const { return capacity_ << 1; }
        size_t size() const { return size_; }
        char* ptr() { return ptr_; }
        const char* ptr_const() const { return ptr_; }

        void set_capacity(size_t capa) {
            assert((capa & 1) == 0);
            capacity_ &= 1;
            capacity_ |= capa;
        }
        void set_size(size_t n) { size_ = n; }
        void set_ptr(char* p) { ptr_ = p; }
    };

private:
    union {
        unsigned char mode : 1;
        unsigned char bytes[24];
        sso_storage sso;
        heap_storage heap;
    } storage{};
};

#endif // _NZD_STRING_HPP_
