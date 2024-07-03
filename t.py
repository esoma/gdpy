
from enum import IntEnum
from typing import Final, Any

class Object:
    pass
    

    NOTIFICATION_POSTINITIALIZE: Final = 0

    NOTIFICATION_PREDELETE: Final = 1

    NOTIFICATION_EXTENSION_RELOADED: Final = 2



    class ConnectFlags:

        CONNECT_DEFERRED = 1

        CONNECT_PERSIST = 2

        CONNECT_ONE_SHOT = 4

        CONNECT_REFERENCE_COUNTED = 8





    def get_class(

        self,


    ) -> (

    Any

    ):
        pass


    def is_class(

        self,


        cls: Any,

    ) -> (

    Any

    ):
        pass


    def set(

        self,


        property: Any,

        value: Any,

    ) -> (

    None

    ):
        pass


    def get(

        self,


        property: Any,

    ) -> (

    Any

    ):
        pass


    def set_indexed(

        self,


        property_path: Any,

        value: Any,

    ) -> (

    None

    ):
        pass


    def get_indexed(

        self,


        property_path: Any,

    ) -> (

    Any

    ):
        pass


    def get_property_list(

        self,


    ) -> (

    Any

    ):
        pass


    def get_method_list(

        self,


    ) -> (

    Any

    ):
        pass


    def property_can_revert(

        self,


        property: Any,

    ) -> (

    Any

    ):
        pass


    def property_get_revert(

        self,


        property: Any,

    ) -> (

    Any

    ):
        pass


    def notification(

        self,


        what: Any,

        reversed: Any,

    ) -> (

    None

    ):
        pass


    def to_string(

        self,


    ) -> (

    Any

    ):
        pass


    def get_instance_id(

        self,


    ) -> (

    Any

    ):
        pass


    def set_script(

        self,


        script: Any,

    ) -> (

    None

    ):
        pass


    def get_script(

        self,


    ) -> (

    Any

    ):
        pass


    def set_meta(

        self,


        name: Any,

        value: Any,

    ) -> (

    None

    ):
        pass


    def remove_meta(

        self,


        name: Any,

    ) -> (

    None

    ):
        pass


    def get_meta(

        self,


        name: Any,

        default: Any,

    ) -> (

    Any

    ):
        pass


    def has_meta(

        self,


        name: Any,

    ) -> (

    Any

    ):
        pass


    def get_meta_list(

        self,


    ) -> (

    Any

    ):
        pass


    def add_user_signal(

        self,


        signal: Any,

        arguments: Any,

    ) -> (

    None

    ):
        pass


    def has_user_signal(

        self,


        signal: Any,

    ) -> (

    Any

    ):
        pass


    def emit_signal(

        self,


        signal: Any,

    ) -> (

    Any

    ):
        pass


    def call(

        self,


        method: Any,

    ) -> (

    Any

    ):
        pass


    def call_deferred(

        self,


        method: Any,

    ) -> (

    Any

    ):
        pass


    def set_deferred(

        self,


        property: Any,

        value: Any,

    ) -> (

    None

    ):
        pass


    def callv(

        self,


        method: Any,

        arg_array: Any,

    ) -> (

    Any

    ):
        pass


    def has_method(

        self,


        method: Any,

    ) -> (

    Any

    ):
        pass


    def has_signal(

        self,


        signal: Any,

    ) -> (

    Any

    ):
        pass


    def get_signal_list(

        self,


    ) -> (

    Any

    ):
        pass


    def get_signal_connection_list(

        self,


        signal: Any,

    ) -> (

    Any

    ):
        pass


    def get_incoming_connections(

        self,


    ) -> (

    Any

    ):
        pass


    def connect(

        self,


        signal: Any,

        callable: Any,

        flags: Any,

    ) -> (

    Any

    ):
        pass


    def disconnect(

        self,


        signal: Any,

        callable: Any,

    ) -> (

    None

    ):
        pass


    def is_connected(

        self,


        signal: Any,

        callable: Any,

    ) -> (

    Any

    ):
        pass


    def set_block_signals(

        self,


        enable: Any,

    ) -> (

    None

    ):
        pass


    def is_blocking_signals(

        self,


    ) -> (

    Any

    ):
        pass


    def notify_property_list_changed(

        self,


    ) -> (

    None

    ):
        pass


    def set_message_translation(

        self,


        enable: Any,

    ) -> (

    None

    ):
        pass


    def can_translate_messages(

        self,


    ) -> (

    Any

    ):
        pass


    def tr(

        self,


        message: Any,

        context: Any,

    ) -> (

    Any

    ):
        pass


    def tr_n(

        self,


        message: Any,

        plural_message: Any,

        n: Any,

        context: Any,

    ) -> (

    Any

    ):
        pass


    def is_queued_for_deletion(

        self,


    ) -> (

    Any

    ):
        pass


    def cancel_free(

        self,


    ) -> (

    None

    ):
        pass

