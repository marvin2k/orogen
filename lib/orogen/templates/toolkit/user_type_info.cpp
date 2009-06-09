
<% begin %>
    std::ostream& operator << (std::ostream& io, <%= type.cxx_basename %> const& data) {
        <% if opaque_specification(type).needs_copy? %>
        <%= intermediate_type.cxx_name %> temp;
        <%= component.name %>::to_intermediate(temp, data);
        <% else %>
        <%= intermediate_type.cxx_name %> const& temp = <%= component.name %>::to_intermediate(data);
        <% end %>

        io << temp;
        return io;
    }

    std::istream& operator >> (std::istream& io, <%= type.cxx_basename %>& data) {
        return io;
    }

    struct <%= type.method_name(false) %>TypeInfo :
	public RTT::TemplateTypeInfo<<%= type.cxx_basename %>, true>
    {
        <%= type.method_name(false) %>TypeInfo()
	    : RTT::TemplateTypeInfo<<%= type.cxx_basename %>, true>("<%= type.full_name %>") {}

	static bool doDecompose(const <%= type.cxx_basename %>& value, RTT::PropertyBag& target_bag) {
            <% if opaque_specification(type).needs_copy? %>
            <%= intermediate_type.cxx_name %> temp;
            <%= component.name %>::to_intermediate(temp, value);
            <% else %>
            <%= intermediate_type.cxx_name %> const& temp = <%= component.name %>::to_intermediate(value);
            <% end %>
            return <%= intermediate_type.cxx_namespace %><%= intermediate_type.method_name(false) %>TypeInfo::doDecompose(temp, target_bag);
        }
	bool decomposeTypeImpl(const <%= type.cxx_basename %>& value, RTT::PropertyBag& target_bag) const {
	    target_bag.setType("<%= type.full_name %>");
            return doDecompose(value, target_bag);
	}

	static bool doCompose(const RTT::PropertyBag& bag, <%= type.cxx_basename %>& out) {
            <% if opaque_specification(type).needs_copy? %>
                <%= intermediate_type.cxx_name %> temp;
                if (! <%= intermediate_type.cxx_namespace %><%= intermediate_type.method_name(false) %>TypeInfo::doCompose(bag, temp))
                    return false;

                <%= component.name %>::from_intermediate(out, temp);
            <% else %>
                std::auto_ptr< <%= intermediate_type.cxx_name %> > temp(new <%= intermediate_type.cxx_name %>);
                if (! <%= intermediate_type.cxx_namespace %><%= intermediate_type.method_name(false) %>TypeInfo::doCompose(bag, *temp))
                    return false;

                if (<%= component.name %>::from_intermediate(out, temp.get()))
                    temp.release();
            <% end %>
            return true;
        }

	bool composeTypeImpl(const RTT::PropertyBag& bag, <%= type.cxx_basename %>& out) const {
            return doCompose(bag, out);
	}
    };
<% rescue TypeError => e
    raise TypeError, "cannot include #{type.name} in the toolkit: #{e}"
   end %>

