    BEGIN;

    select '{"a": {"b": 1}}'::jsonb <=> '{"a.b" : 1}';
	
	select '{"a": {"b": 1}}'::jsonb <=> '{"a.b" : { $lt: 1 } }';

	select '{"a": {"b": [1,2,3]}}'::jsonb <=> '{ "a.b" : { $in: [ 1,2,3,4 ] } }';
	
	select '{"a": {"b": [1,2,3]}}'::jsonb <=> '{ "a.b" : { $nin: [ 1,2,3,4 ] } }';

	select '{"a": {"b": [1,2,3]}}'::jsonb <=> '{ "a.b" : [1,2,3] }';

	select '{ "quantity" : 200, "price" : 10 }'::jsonb <=> '{ $and: [ { quantity: { $lt: 20 } }, { price: 10 } ] }';

	select '{ "quantity" : 200, "price" : 10 }'::jsonb <=> '{ $and: [ { quantity: { $lt: 20 } }, { price: 100 } ] }';
	
	select '{ "quantity" : 200, "price" : 10 }'::jsonb <=> '{ $or: [ { quantity: { $lt: 20 } }, { price: 10 } ] }';
	
	select '{ "quantity" : 200, "price" : 10 }'::jsonb <=> '{ $or: [ { quantity: { $lt: 20 } }, { price: 100 } ] }';

	select '{ "quantity" : 5, "price" : 100 }'::jsonb <=> '{ price: { $not: { $gt: 1.99 } } }';
	
	select '{ "quantity" : 5, "price" : 1 }'::jsonb <=> '{ price: { $not: { $gt: 1.99 } } }';

    END;
