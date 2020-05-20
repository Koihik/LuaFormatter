function M:add_permission(
				admin_token,
				user_id,
				occ_token,
				permission,
				aksdgkajgk,
				aghadkfjgadfjgk,
				akhfjdkahjkadfj,
				akhjdfkajh,
				kasfjdgkadjgfk,
				akgjfdkajg
)
	auxilary.assert_type(admin_token, "string")
	auxilary.assert_type(user_id, "string")
	auxilary.assert_type(occ_token, "string")
	auxilary.assert_type(permission, "string")

	local output = self.base:call_request(
               					"POST", "/user/permission/add", {["Content-Type"] = auxilary.mime_types.json, token = admin_token},
               					cjson.encode({user_id = user_id, occ_token = occ_token, permission = permission})
               	)

	local out, err = self.base.check_response_json(output, {"occ_token"})
	if out == nil then
		return nil, err
	end
	return out.occ_token
end
