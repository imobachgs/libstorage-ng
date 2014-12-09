#ifndef USING_H
#define USING_H


#include "storage/Holders/Holder.h"


namespace storage_bgl
{

    class Using : public Holder
    {
    public:

	static Using* create(Devicegraph* devicegraph, const Device* source, const Device* target);
	static Using* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual const char* get_classname() const override { return "Using"; }

	virtual Using* clone() const override;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    private:

	Using(Impl* impl);

    };

}

#endif