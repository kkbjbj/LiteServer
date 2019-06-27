function clone(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for key, value in pairs(object) do
            new_table[_copy(key)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end

function IsSuperCtor(instance)
    local super = instance.super

    while super do
        if instance.ctor == super.ctor then
            return true
        end
        super = super.super
    end
    return false
end

--Create an class.

AllClassList = AllClassList or {}
function class(classname, super)
    local superType = type(super)
    local cls

    if superType ~= "function" and superType ~= "table" then
        superType = nil
        super = nil
    end

    if superType == "function" or (super and super.__ctype == 1) then
        -- inherited from native C++ Object
        cls = {}

        if superType == "table" then
            -- copy fields from super
            for k,v in pairs(super) do cls[k] = v end
            cls.__create = super.__create
            cls.super    = super
        else
            cls.__create = super
            cls.ctor    = function() end
        end

        cls.__cname = classname
        cls.__ctype = 1

        function cls.New(...)
            local instance = cls.__create(...)
            -- copy fields from class to native object
            for k,v in pairs(cls) do instance[k] = v end
            instance.class = cls
            local tabCtor = {}
            if not IsSuperCtor(instance) then
                tabCtor = {instance}
            end

            local tSuper = instance.super;
            while tSuper do
                if not IsSuperCtor(tSuper) then
                    table.insert( tabCtor,tSuper)
                end
                tSuper = tSuper.super;
            end

            local tcount = #tabCtor
            for i=tcount,1,-1 do
                tabCtor[i].ctor(instance,...)
            end

            return instance
        end

    else
        -- inherited from Lua Object
        if super then
            cls = clone(super)
            cls.super = super
        else
            cls = {ctor = function() end}
        end

        cls.__cname = classname
        cls.__ctype = 2 -- lua
        cls.__index = cls

        function cls.New(...)
            local instance = setmetatable({}, cls)
            instance.class = cls

            local tabCtor = {}
            if not IsSuperCtor(instance) then
                tabCtor = {instance}
            end

            local tSuper = instance.super;
            while tSuper do
                if not IsSuperCtor(tSuper) then
                    table.insert( tabCtor,tSuper)
                end
                tSuper = tSuper.super;
            end

            local tcount = #tabCtor
            for i=tcount,1,-1 do
                tabCtor[i].ctor(instance,...)
            end

            return instance
        end
    end
	
	AllClassList[classname] = cls
    return cls
end

function ReloadAllClass()
    warn("ReloadAllClass Start...")
    for Name, gClass in pairs(AllClassList) do
        local CC = _G[gClass.__cname]

        for k, v in pairs(CC) do
            if type(v) == 'function' then
                gClass[k] = v
            end
        end
    end

    warn("ReloadAllClass End...")
end
