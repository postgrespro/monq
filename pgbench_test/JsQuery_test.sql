    BEGIN;

    select '{"a": {"b": 1}}'::jsonb @@ 'a.b = 1';
	
	select '{"a": {"b": 1}}'::jsonb @@ 'a.b < 1';

	select '{"a": {"b": [1,2,3]}}'::jsonb @@ 'a.b @> [ 1,2,3,4 ]';
	
	select '{"a": {"b": [1,2,3]}}'::jsonb @@ 'a.b <@ [ 1,2,3,4 ]';

	select '{"a": {"b": [1,2,3]}}'::jsonb @@ 'a.b = [1,2,3]';

	select '{ "quantity" : 200, "price" : 10 }'::jsonb @@ '(quantity < 20 AND price = 100)';

	select '{ "quantity" : 200, "price" : 10 }'::jsonb @@ '(quantity < 20 AND price = 10)';

	select '{ "quantity" : 200, "price" : 10 }'::jsonb @@ '(quantity < 20 OR price = 100)';

	select '{ "quantity" : 200, "price" : 10 }'::jsonb @@ '(quantity < 20 OR price = 10)';

	select '{ "quantity" : 5, "price" : 100 }'::jsonb @@ 'NOT (price > 1.99)';
	
	select '{ "quantity" : 5, "price" : 1 }'::jsonb @@ 'NOT (price > 1.99)';

    END;
