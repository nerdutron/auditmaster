
CREATE OR REPLACE view user_session_current AS SELECT * FROM user_session WHERE session_expires > now();

CREATE OR REPLACE view user_session_log AS SELECT login,session_start FROM user_session, security_public where user_session.user_id = security_public.user_id ORDER BY session_start desc;
