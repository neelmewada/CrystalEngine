#pragma once

#define CE_DEPRECATED(Version, Message) [[deprecated(Message " Please update your code to not use the deprecated API.")]]

#define CE_NO_COPY(Class) Class(Class& Copy) = delete

#define CE_STATIC_CLASS(Class) Class() = delete; ~Class() = delete
