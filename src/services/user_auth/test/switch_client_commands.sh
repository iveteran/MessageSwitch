# Run below commands in Switch client
ss_req_svc --file ua_test_data_user_auth.json --svc_type 101 --svc_cmd 4        # create user
ss_req_svc --file ua_test_data_user_auth.json --svc_type 101 --svc_cmd 1        # login
ss_req_svc --file ua_test_data_user_session.json --svc_type 101 --svc_cmd 3     # verify
ss_req_svc --file ua_test_data_user_session.json --svc_type 101 --svc_cmd 2     # logout
ss_req_svc --file ua_test_data_user_session.json --svc_type 101 --svc_cmd 5     # delete user
