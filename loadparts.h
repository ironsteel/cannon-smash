/***********************************************************************
 *	z:/wata/src/a/csmash/loadparts.h
 *	$Id: loadparts.h,v 1.7 2002/03/05 14:21:21 yotsuya Exp $
 *
 *	Copyright by ESESoft.
 *
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions
 *	are met:
 *
 *	Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer. 
 *
 *	Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer
 *	in the documentation and/or other materials provided with the
 *	distribution. 
 *
 *	The name of the author may not be used to endorse or promote
 *	products derived from this software without specific prior written
 *	permission. 
 *
 *	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *	ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***********************************************************************/
#ifndef __Yotsuya_ESESoft_07897981__loadparts_h__INCLUDED__
#define __Yotsuya_ESESoft_07897981__loadparts_h__INCLUDED__
/***********************************************************************/
#include <stdarg.h>
#include <map>
#include <list>
#include <string>
/* __BEGIN__BEGIN__ */

/***********************************************************************
 *	Class parts (root)
 ***********************************************************************/
class texture_parts;
class polyhedron_parts;
class anim_parts;
class body_parts;

class parts
{
public:
    typedef std::basic_string<char> string;
    class error {
    public:
	const char* str;
	
	inline error(const char *str) : str(str) {}
	const char* what() const { return str; }
    };
    class verror : public error
    {
    public:
        char buf[508];  // 512 - sizeof(char*)

        inline verror(int lineno, const char *fmt, ...) : error(buf) {
            int l = snprintf(buf, sizeof(buf), "%d: ", lineno);
            va_list arg;
            va_start(arg, fmt);
            vsnprintf(&buf[l], sizeof(buf)-l, fmt, arg);
            va_end(arg);
        }            
        inline verror(const char *fmt, ...) : error(buf) {
            va_list arg;
            va_start(arg, fmt);
            vsnprintf(buf, sizeof(buf), fmt, arg);
            va_end(arg);
        }
    };
    
    enum symbol_t {
	sym_unknown = 0,
	sym_null,
	sym_load, sym_create,
	sym_polyhedron,	sym_anim, sym_texture,
	sym_body,
    };

    inline parts(const char *str) : name(str) {}
    virtual ~parts() {}

    virtual bool load(const char*) = 0;
    virtual symbol_t type() const = 0;
    virtual const char *typestr() const { return sym2str(type()); }
    virtual bool assign(parts* obejct) { return false; }

    virtual bool realize() { return true; }
    virtual void unrealize() {}

    static symbol_t getsym(const char *str);
    static const char* sym2str(symbol_t);

    static parts* getobject(const char* name);
    static void clearobjects();
    static bool loadobjects(const char *str);

    static bool realizeobjects();
    static void unrealizeobjects();

protected:
    static bool addobject(const char* name, parts*);
    static bool delobject(const char *name);
    static bool loadfile(const char *str);

private:
    static bool load_create(int lineno, int ac, const char *av[], int *optind);
    static bool load_load(int lineno, int ac, const char *av[], int *optind);
    static bool load_polyhedron(int lineno, polyhedron_parts*,
                                int ac, const char *av[], int *ind);
    static bool load_anim(int lineno, anim_parts*,
			  int ac, const char *av[], int *ind);
public:
    string name;
};

/***********************************************************************
 *	Class parts_map
 ***********************************************************************/
class parts_map : public std::map<parts::string, parts*>
{
public:
    typedef std::map<parts::string, parts*> super;

    ~parts_map() {
	clear();
    }	
    void clear() {
	for (iterator i = begin(); i != end(); ++i) delete i->second;
	super::clear();
    }
};

/***********************************************************************
 *	Class texture_parts
 ***********************************************************************/
class texture_parts : public parts
{
public:
    typedef GLuint object_t;
    GLuint object;
    string filename;

    inline texture_parts(const char *name) : parts(name), object(0) {}
    virtual ~texture_parts() { unrealize(); }
    virtual symbol_t type() const { return sym_texture; }
    virtual bool load(const char *str);

public:
    // Textures must be realized before glBindTextures().
    // realize() will fail if GL library is not initialized yet.
    virtual bool realize();
    virtual void unrealize();
};

/***********************************************************************
 *	Class polyhedron_parts
 ***********************************************************************/
class polyhedron_parts : public parts
{
public:
    typedef polyhedron object_t;
    polyhedron *object;
    texture_parts *tex;

    inline polyhedron_parts(const char *name) : parts(name), object(NULL), tex(NULL) {}
    virtual ~polyhedron_parts() { delete object; }
    virtual symbol_t type() const { return sym_polyhedron; }
    virtual bool assign(parts*);
    virtual bool load(const char* str);

public:
    void render() const;
    void renderWire(const vector3F &origin) const;
};

/***********************************************************************
 *	Class anim_parts
 ***********************************************************************/
class anim_parts : public parts
{
public:
    typedef affineanim object_t;
    affineanim *object;
    std::list<polyhedron_parts*> poly;

    inline anim_parts(const char *name) : parts(name), object(NULL) {}
    virtual ~anim_parts() { delete object; }
    virtual symbol_t type() const { return sym_anim; }
    virtual bool assign(parts*);
    virtual bool load(const char* str);

public:
    void render(int frame) const;
    void renderWire(int frame) const;
};

/***********************************************************************
 *	Class body_parts
 ***********************************************************************/
class body_parts : public parts
{
public:
    typedef std::list<affinemotion*> object_t;
    std::list<anim_parts*> object;

    inline body_parts(const char *name) : parts(name) {}
    virtual ~body_parts() {}
    inline symbol_t type() const { return sym_body; }
    virtual bool assign(parts*);
    virtual bool load(const char* str) { return false; }

public:
    void render(int frame) const;
    void renderWire(int frame) const;
};

/* __END__END__ */
/***********************************************************************/
#endif
/***********************************************************************
 *	END OF loadparts.h
 ***********************************************************************/
